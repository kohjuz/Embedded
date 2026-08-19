/*
 * photo_demo.c — 相册程序 (动态库版)
 *
 * 对比原来的 photo.c (400多行: 自己解析BMP头/行对齐/倒序拷贝/
 * jpg2rgb/fb_open/居中缩放/触摸翻页...):
 * 现在链接上两个库, 只剩核心业务逻辑, 约 60 行。
 *
 * 功能:
 *   1. 扫描目录, 收集所有 .jpg/.jpeg/.bmp 图片
 *   2. 触摸屏左右滑动翻页 ( /dev/input/event6, 和原来 photo.c 一样 )
 *      左滑 = 下一张, 右滑 = 上一张
 *   3. photo_display() 一行显示图片, 自动判断 BMP/JPG
 *
 * 编译(由 Makefile.arm 完成):
 *   make -f Makefile.arm deploy APP=photo_demo.c
 *   或直接:
 *   aarch64-linux-gnu-gcc photo_demo.c -L arm -lpicture_show -llcd_io \
 *       -lpicture_io -Wl,-rpath,/root -o arm/photo_demo
 *
 * 运行:
 *   ./photo_demo [图片目录]        # 默认扫描当前目录
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include "picture_show.h"

#define MAX_PHOTOS 256
#define TOUCH_DEV  "/dev/input/event6"

static char photo_list[MAX_PHOTOS][256];
static int  photo_count = 0;

/* 判断文件名是否为图片: .jpg/.jpeg/.bmp */
static int is_photo_file(const char *name)
{
    if (name == NULL || name[0] == '.')
        return 0;
    const char *p = strrchr(name, '.');
    if (p == NULL)
        return 0;
    return (strcasecmp(p, ".jpg")  == 0 ||
            strcasecmp(p, ".jpeg") == 0 ||
            strcasecmp(p, ".bmp")  == 0);
}

/* 扫描目录, 把图片文件名收集到列表 */
static void scan_photos(const char *dir)
{
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        perror("opendir");
        exit(1);
    }
    struct dirent *ent;
    while ((ent = readdir(dp)) != NULL) {
        if (photo_count >= MAX_PHOTOS)
            break;
        if (is_photo_file(ent->d_name))
            snprintf(photo_list[photo_count++], 256, "%s/%s",
                     dir, ent->d_name);
    }
    closedir(dp);
}

/* 触摸翻页: 左滑=下一张, 右滑=上一张 (单击不动则不翻页) */
static void touch_loop(void)
{
    int tp = open(TOUCH_DEV, O_RDWR);
    if (tp < 0) {
        perror(TOUCH_DEV);
        printf("打不开触摸屏, 只能看第一张\n");
        return;
    }

    int cur = 0, pressed = 0;
    int cur_x = 0, cur_y = 0;      /* 触摸当前位置 */
    int press_x = 0, press_y = 0;  /* 按下时的位置   */
    struct input_event ev;

    photo_display(photo_list[cur]);   /* 先显示第一张 */

    while (1) {
        if (read(tp, &ev, sizeof(ev)) <= 0)
            continue;

        if (ev.type == EV_ABS && ev.code == ABS_X)
            cur_x = ev.value;
        else if (ev.type == EV_ABS && ev.code == ABS_Y)
            cur_y = ev.value;
        else if (ev.type == EV_KEY && ev.code == BTN_TOUCH) {
            if (ev.value == 1) {                 /* 按下: 记录起点 */
                pressed = 1;
                press_x = cur_x;
                press_y = cur_y;
            } else if (ev.value == 0 && pressed) { /* 抬手: 判断滑动 */
                pressed = 0;
                int dx = cur_x - press_x;
                int dy = cur_y - press_y;
                /* 水平位移大且超过阈值 → 翻页 */
                if (abs(dx) > abs(dy) && abs(dx) > 50) {
                    if (dx < 0)
                        cur = (cur + 1) % photo_count;           /* 左滑 */
                    else
                        cur = (cur - 1 + photo_count) % photo_count; /* 右滑 */
                    printf("[%d/%d] %s\n", cur + 1, photo_count,
                           photo_list[cur]);
                    photo_display(photo_list[cur]);
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    const char *dir = (argc > 1) ? argv[1] : ".";
    scan_photos(dir);

    if (photo_count == 0) {
        printf("目录 %s 里没有 .jpg/.jpeg/.bmp 图片\n", dir);
        return 1;
    }
    printf("找到 %d 张图片 | 触摸左滑=下一张 右滑=上一张\n", photo_count);

    touch_loop();
    return 0;
}
