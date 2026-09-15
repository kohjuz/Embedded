/* slide.c - 幻灯片模块（应用层）
 *
 * 功能：
 *   1. 扫描 ./lib_photo 生成图片列表（jpg / bmp）
 *   2. 列表页：文件名 + 类型，翻页，点击进入大图
 *   3. 大图页：等比缩放显示 + 信息叠加 + 3 秒自动播放
 *             + 点击左右翻页 + 滑动翻页 + 顶部 BACK
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>

#include "app.h"
#include "libdisp.h"
#include "touch.h"

#define PHOTO_DIR  "./lib_photo"
#define MAX_PHOTOS 256

static char g_photos[MAX_PHOTOS][256];
static int  g_n = 0;

/* 判断是否图片文件，返回 1=JPG 2=BMP 0=否 */
static int photo_type(const char *name)
{
    const char *p = strrchr(name, '.');
    if (!p || name[0] == '.')           /* 排除隐藏文件 */
        return 0;
    if (strcasecmp(p, ".jpg") == 0 || strcasecmp(p, ".jpeg") == 0)
        return 1;
    if (strcasecmp(p, ".bmp") == 0)
        return 2;
    return 0;
}

static int cmp_photo(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

/* 图片扫描与浏览（slide 和 touch_photo 共用，所以不 static） */

int photo_count(void)
{
    return g_n;
}

void photo_scan(void)
{
    g_n = 0;
    DIR *dir = opendir(PHOTO_DIR);
    if (!dir) {
        perror("opendir lib_photo");
        return;
    }
    struct dirent *e;
    while ((e = readdir(dir)) != NULL && g_n < MAX_PHOTOS) {
        if (!photo_type(e->d_name))
            continue;
        snprintf(g_photos[g_n], 256, "%s/%s", PHOTO_DIR, e->d_name);
        g_n++;
    }
    closedir(dir);
    qsort(g_photos, g_n, 256, cmp_photo);
    printf("[slide] %d photos in %s\n", g_n, PHOTO_DIR);
}

static const char *base_name(const char *path)
{
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

/* ========== 大图浏览页：返回 -1 表示要回菜单 ==========
 * auto_ms > 0: 自动播放（超时自动下一张）；auto_ms <= 0: 纯手动
 * 交互：点左半/右滑 = 上一张，点右半/左滑 = 下一张，左上角 BACK
 */
int view_loop(int idx, int auto_ms)
{
    while (1) {
        disp_clear(&g_fb);
        disp_image_fit(&g_fb, g_photos[idx]);

        /* 顶部信息条（图片信息叠加） */
        disp_fill_rect(&g_fb, 0, 0, 560, 28, 0, 0, 0);
        char info[128];
        snprintf(info, sizeof(info), "BACK  %d/%d  %s",
                 idx + 1, g_n, base_name(g_photos[idx]));
        disp_text(&g_fb, 10, 7, info);

        /* 底部提示条 */
        disp_fill_rect(&g_fb, 0, g_fb.height - 22, 460, g_fb.height, 0, 0, 0);
        disp_text(&g_fb, 10, g_fb.height - 16,
                  auto_ms > 0 ? "< left  right >   auto next" :
                                "< left  right >   manual");

        int x, y, x2, y2;
        int r = touch_read(auto_ms > 0 ? auto_ms : -1, &x, &y, &x2, &y2);
        if (r == 0) {                       /* 超时：自动下一张 */
            if (auto_ms > 0) {
                idx = (idx + 1) % g_n;
                continue;
            }
            continue;                       /* 手动模式是 -1 无限等，不会到这 */
        }
        if (r == 2) {                       /* 滑动翻页 */
            if (x2 < x)                     /* 左滑 → 下一张 */
                idx = (idx + 1) % g_n;
            else
                idx = (idx - 1 + g_n) % g_n;
            continue;
        }
        if (r == 1) {
            if (y < 40 && x < 130)
                return -1;                  /* 点 BACK 回菜单 */
            if (x < g_fb.width / 2)
                idx = (idx - 1 + g_n) % g_n;
            else
                idx = (idx + 1) % g_n;
        }
    }
}

/* ========== 幻灯片：点进直接看大图 ==========
 * 不经过文字列表页，直接进入大图自动播放：
 *   点左半边/右滑 = 上一张，点右半边/左滑 = 下一张
 *   3 秒无操作自动下一张，左上角 BACK 返回主界面
 */
app_mode_t slide_run(void)
{
    photo_scan();                       /* 每次进幻灯片重新扫描 lib_photo */

    if (g_n <= 0) {                     /* 目录没有图片：提示后返回 */
        disp_clear(&g_fb);
        disp_text(&g_fb, 20, 250, "no photo in lib_photo");
        disp_text(&g_fb, 20, 280, "tap to back");
        touch_read(-1, NULL, NULL, NULL, NULL);
        return MODE_MENU;
    }

    view_loop(0, 3000);                 /* 第 0 张开始，3 秒自动播放 */
    return MODE_MENU;                   /* 大图里点 BACK 后回主界面 */
}
