/* music.c - 音乐模块（应用层）
 *
 * 功能：
 *   1. 扫描 ./music_lib 目录下的 wav 文件，生成曲目列表
 *   2. pthread 播放线程 + aplay 子进程 → 播放不阻塞触摸
 *   3. 音乐列表界面（点歌播放/停止，翻页，返回）
 *
 * 播放原理：
 *   aplay 是开发板系统自带的 ALSA 播放工具，直接播 WAV。
 *   这里用 fork() 起 aplay 子进程播放，程序主体继续响应触摸；
 *   停止时给子进程发 SIGTERM。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/wait.h>

#include "app.h"
#include "libdisp.h"
#include "touch.h"
#include "music.h"

#define MUSIC_DIR  "./music_lib"
#define MAX_SONGS  64

static char g_songs[MAX_SONGS][256];
static int  g_count = 0;
static int  g_cur   = -1;        /* 当前曲目下标 */

static pthread_t      g_pt;
static volatile int   g_playing = 0;
static volatile pid_t g_pid     = -1;

/* 按名字排序，列表稳定不乱跳 */
static int cmp_song(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

/* ========== 播放线程：fork aplay，等它放完 ========== */

/* 设置输出路由。返回 1=成功 0=失败。
 * 注意：有的板子 amixer 是 busybox 版，对 enum 控件（Playback Path）
 * 传名字会失败，必须传数字索引（SPK_HP=6, HP=3），两种都试。 */
static int set_path(const char *name, int idx)
{
    char cmd[160];
    snprintf(cmd, sizeof(cmd),
             "amixer -q set 'Playback Path' %s 2>/dev/null", name);
    if (system(cmd) == 0) {
        printf("[music] path: %s\n", name);
        return 1;
    }
    snprintf(cmd, sizeof(cmd),
             "amixer -q set 'Playback Path' %d 2>/dev/null", idx);
    if (system(cmd) == 0) {
        printf("[music] path: %s (idx %d)\n", name, idx);
        return 1;
    }
    return 0;
}

/* 播放前把输出通路准备好：
 *  1. 路由打开：RK817 的 Playback Path 默认是 OFF（静音！），
 *     不设的话 aplay 数据照送、codec 照吞，就是没声音。
 *     设 SPK_HP = 耳机 + 喇叭同时出，插没插耳机都能听到；
 *     个别固件没有 SPK_HP，退而求其次设 HP。
 *  2. 音量压到安全档（20%），防止刚接上设备突然爆音。
 * RK817 的控件名不同固件略有差异（HP Volume / Speaker 等），
 * 逐个尝试，存在的控件就设一次，不存在的会被 amixer 拒绝（非 0）。 */
static void volume_safe(void)
{
    /* 1. 先开路由（最关键！OFF 状态下声音全被吞掉） */
    if (!set_path("SPK_HP", 6))        /* 耳机+喇叭同时出 */
        set_path("HP", 3);             /* 固件没 SPK_HP 就只走耳机 */

    /* 2. softvol 软件音量层（asound.conf 的 default 设备链）：
     *    aplay 默认设备经过 Soft volume PCM，控件叫 Master Playback Volume，
     *    初始可能是 0（静音）。拉满它——真正响度由硬件 Master 控制。 */
    if (system("amixer -q -D default set 'Master Playback Volume' 100% 2>/dev/null") == 0)
        printf("[music] volume: softvol Master Playback Volume -> 100%%\n");

    /* 3. 再压硬件音量 */
    const char *controls[] = {
        "Master",                       /* RK817 主音量（当前板子确认） */
        "HP Volume", "Speaker Volume", "SPK Volume",
        "Headphone", "Speaker", "PCM"
    };
    for (size_t i = 0; i < sizeof(controls) / sizeof(controls[0]); i++) {
        char cmd[160];
        snprintf(cmd, sizeof(cmd),
                 "amixer -q set '%s' 20%% 2>/dev/null", controls[i]);
        if (system(cmd) == 0)
            printf("[music] volume: %s -> 20%% (safe)\n", controls[i]);
    }
}

static void *play_thread(void *arg)
{
    char *path = arg;
    volume_safe();                      /* 防爆音：先压音量再开播 */
    pid_t pid = -1;
    int st = 0;

    /* 声卡可能刚被占用（上一次播放刚停、或外部 aplay 抢着），
     * aplay 打开失败就等 150ms 重试，最多 5 次 */
    for (int try = 0; try < 5; try++) {
        if (try > 0) {
            /* 重试前清场：可能有孤儿 aplay（上次 Ctrl+C 杀 frame 时
             * 留下的子进程）一直占着声卡。music_init 只清过一次，
             * 这里再保险，杀干净再抢设备。 */
            system("killall aplay 2>/dev/null");
            usleep(100000);
        }
        pid = fork();
        if (pid == 0) {                 /* 子进程：变成 aplay */
            execlp("aplay", "aplay", "-q", path, (char *)NULL);
            _exit(127);                 /* execlp 失败才走到这 */
        }
        g_pid = pid;
        waitpid(pid, &st, 0);           /* 等 aplay 退出 */
        if (WIFEXITED(st) && WEXITSTATUS(st) == 0)
            break;                      /* 自然放完 = 成功 */
        /* aplay 失败（多半是 busy）或中途被杀，看看还要不要重试 */
        if (g_pid != pid)
            break;                      /* 播放被 music_stop 停了 */
        usleep(150000);
    }

    /* 关键：确认自己还是"现任"播放线程才清状态。
     * 否则快速切歌时，旧线程退出会误清新歌的播放状态。 */
    if (g_pid == pid) {
        g_pid = -1;
        g_playing = 0;
        g_cur = -1;
    }
    free(path);
    return NULL;
}

int music_init(void)
{
    /* 启动时清场：上次异常退出 / 手动测试可能残留 aplay 进程，
     * 它们占着声卡，新播放会报 "Device or resource busy" */
    system("killall aplay 2>/dev/null");

    g_count = 0;
    DIR *dir = opendir(MUSIC_DIR);
    if (!dir) {
        perror("opendir music_lib");
        return 0;
    }
    struct dirent *e;
    while ((e = readdir(dir)) != NULL && g_count < MAX_SONGS) {
        const char *ext = strrchr(e->d_name, '.');
        if (!ext)
            continue;
        if (strcasecmp(ext, ".wav") != 0)
            continue;
        snprintf(g_songs[g_count], 256, "%s/%s", MUSIC_DIR, e->d_name);
        g_count++;
    }
    closedir(dir);
    qsort(g_songs, g_count, 256, cmp_song);
    printf("[music] %d songs in %s\n", g_count, MUSIC_DIR);
    return g_count;
}

int music_count(void)      { return g_count; }
int music_is_playing(void) { return g_playing; }
int music_current(void)    { return g_cur; }

void music_play(int idx)
{
    if (idx < 0 || idx >= g_count)
        return;
    music_stop();
    g_cur = idx;
    g_playing = 1;
    pthread_create(&g_pt, NULL, play_thread, strdup(g_songs[idx]));
    printf("[music] play: %s\n", g_songs[idx]);
}

void music_stop(void)
{
    pid_t cur = g_pid;                  /* 取快照，避免竞态 */
    if (cur > 0) {
        kill(cur, SIGTERM);
        /* kill 是异步的：SIGTERM 发出后旧 aplay 要花一点时间
         * 退出并释放声卡。不等的话，紧接着的新 aplay 一 fork
         * 就撞上 "Device or resource busy" */
        usleep(150000);                 /* 150ms，给旧进程让路 */
    }
    g_pid = -1;
    g_playing = 0;
    g_cur = -1;
}

/* ========== 音乐列表界面 ========== */
static const char *base_name(const char *path)
{
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

app_mode_t music_run(void)
{
    const int PER = 10;                 /* 每页行数 */
    int page = 0;
    int pages = (g_count + PER - 1) / PER;
    if (pages < 1) pages = 1;

    while (1) {
        disp_clear(&g_fb);

        /* 标题 + 播放状态 */
        disp_text(&g_fb, 20, 14, "Music List");
        char st[128];
        if (g_playing && g_cur >= 0)
            snprintf(st, sizeof(st), "PLAYING: %s", base_name(g_songs[g_cur]));
        else
            snprintf(st, sizeof(st), "tap a song to play / stop");
        disp_text(&g_fb, 20, 42, st);

        /* 曲目列表 */
        for (int i = 0; i < PER; i++) {
            int idx = page * PER + i;
            if (idx >= g_count)
                break;
            int y = 74 + i * 32;
            int active = (idx == g_cur && g_playing);
            if (active)                                  /* 高亮正在播的 */
                disp_fill_rect(&g_fb, 8, y - 4, 660, y + 24, 20, 70, 40);
            char line[96];
            snprintf(line, sizeof(line), "%s %02d  %s",
                     active ? ">>" : "  ", idx + 1, base_name(g_songs[idx]));
            disp_text(&g_fb, 16, y, line);
        }

        /* 底部操作栏 */
        disp_text(&g_fb, 20, 566, "BACK");
        char pg[32];
        snprintf(pg, sizeof(pg), "P %d/%d", page + 1, pages);
        disp_text(&g_fb, 470, 566, pg);
        disp_text(&g_fb, 800, 566, "<PREV  NEXT>");

        int x, y, x2, y2;
        int r = touch_read(-1, &x, &y, &x2, &y2);
        if (r != 1)
            continue;

        if (y >= 550) {                 /* 底部栏 */
            if (x < 130)
                return MODE_MENU;
            if (x >= 800 && x < 910) { if (page > 0) page--; }
            else if (x >= 910)        { if (page < pages - 1) page++; }
            continue;
        }

        int row = (y - 74) / 32;        /* 点到哪一行 */
        int idx = page * PER + row;
        if (row >= 0 && row < PER && idx >= 0 && idx < g_count) {
            if (g_playing && g_cur == idx)
                music_stop();
            else
                music_play(idx);
        }
    }
}
