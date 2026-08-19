/*
 * 显示 JPG 图片示例 —— 只调用 jpeg_display 模块的两个函数
 *
 * 编译: gcc showjpg.c jpeg_display.c -o showjpg -ljpeg
 * 运行: ./showjpg a.jpg         原尺寸显示
 *       ./showjpg a.jpg fit     等比缩放, 整张图显示在屏幕内
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jpeg_display.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <jpgfile> [fit]\n", argv[0]);
        return -1;
    }

    /* 函数1: JPEG文件 → RGB24 */
    struct imageInfo img;
    if (jpg2rgb(argv[1], &img) != 0)
        return -1;

    /* 打开屏幕 */
    FBInfo fb;
    if (fb_open(&fb) != 0) {
        free(img.rgb);
        return -1;
    }
    fb_clear(&fb);

    /* 计算缩放比例: 默认原尺寸; fit = 等比缩放铺满屏幕内 */
    int scale = 1000;   /* 1000 = 100% */
    if (argc >= 3 && strcmp(argv[2], "fit") == 0) {
        int s_x = fb.width  * 1000 / img.width;
        int s_y = fb.height * 1000 / img.height;
        scale = (s_x < s_y) ? s_x : s_y;
        if (scale < 1) scale = 1;
    }

    /* 函数2: RGB24 → 显示到屏幕 */
    fb_show_image(&fb, &img, scale);

    /* 清理 */
    fb_close(&fb);
    free(img.rgb);
    return 0;
}
