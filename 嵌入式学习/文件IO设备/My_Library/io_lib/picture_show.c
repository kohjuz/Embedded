/*
 * picture_show.c — 图片显示动态库 (BMP/JPG 一键显示到 LCD)
 *
 * 功能: 把"解码图片 → 打开屏幕 → 居中缩放 → 上屏 → 清理"整条流程
 *       封装成一行函数, 以后写照片浏览器不用再复制粘贴 photo.c 里
 *       那一大段 bmp_display() 了
 *
 * 依赖另外两个动态库:
 *   libpicture_io.so  解码库 (get_picture: 文件 → RGB24)
 *   liblcd_io.so      LCD显示库 (fb_open/fb_show_rgb/...)
 *
 * 编译成动态库:
 *   gcc -shared -fPIC -o libpicture_show.so picture_show.c \
 *       -L. -llcd_io -lpicture_io -Wl,-rpath,'$ORIGIN'
 *
 * 使用:
 *   #include "picture_show.h"
 *   photo_display("1.jpg");   // 一行搞定
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "picture_io.h"
#include "lcd_io.h"

/* 根据扩展名判断图片类型: 1=JPG, 2=BMP, 0=其他 */
static int picture_type(const char *name)
{
    if (!name) return 0;
    const char *p = strrchr(name, '.');
    if (!p) return 0;
    if (strcasecmp(p, ".jpg") == 0 || strcasecmp(p, ".jpeg") == 0)
        return 1;
    if (strcasecmp(p, ".bmp") == 0)
        return 2;
    return 0;
}

/* 打开屏幕并显示一张已解码的图片, 用完自动清理 */
static int show_picture(char *rgb, int w, int h, int scale)
{
    FBInfo fb;
    if (fb_open(&fb) != 0) {
        free(rgb);
        return -1;
    }
    fb_clear(&fb);
    fb_show_rgb(&fb, rgb, w, h, scale);
    fb_close(&fb);
    return 0;
}

/* ==================================================================
 * 显示 BMP: 原尺寸居中, 超过屏幕的部分裁剪
 * (行为和原来 photo.c 里的 bmp_display() 一样)
 * ================================================================== */
int bmp_display(const char *path)
{
    PictureInfo img;
    if (get_picture(path, &img) != 0)
        return -1;
    return show_picture(img.rgb, img.width, img.height, 1000);
}

/* ==================================================================
 * 显示 JPG: 等比缩放, 铺满屏幕内
 * (行为和原来 photo.c 里 photo_output() 的 JPG 分支一样)
 * ================================================================== */
int jpg_display(const char *path)
{
    PictureInfo img;
    if (get_picture(path, &img) != 0)
        return -1;

    FBInfo fb;
    if (fb_open(&fb) != 0) {
        free(img.rgb);
        return -1;
    }

    /* 等比缩放比例: 铺满屏幕内 (fit) */
    int s_x = fb.width  * 1000 / img.width;
    int s_y = fb.height * 1000 / img.height;
    int scale = (s_x < s_y) ? s_x : s_y;
    if (scale < 1) scale = 1;

    fb_clear(&fb);
    fb_show_rgb(&fb, img.rgb, img.width, img.height, scale);
    fb_close(&fb);

    free(img.rgb);
    return 0;
}

/* ==================================================================
 * 一键显示: 自动判断 BMP/JPG
 * (等价于原来 photo.c 里的 photo_output(), 一行调用)
 * ================================================================== */
int photo_display(const char *path)
{
    int type = picture_type(path);
    if (type == 1)
        return jpg_display(path);
    if (type == 2)
        return bmp_display(path);

    fprintf(stderr, "[photo_display] 不支持的图片格式: %s\n", path);
    return -1;
}

/* ==================================================================
 * 按指定比例显示任意格式图片
 * ================================================================== */
int photo_display_scaled(const char *path, int scale)
{
    PictureInfo img;
    if (get_picture(path, &img) != 0)
        return -1;
    return show_picture(img.rgb, img.width, img.height, scale);
}
