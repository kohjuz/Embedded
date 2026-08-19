/*
 * lcd_io.c — LCD(Framebuffer) 显示动态库
 *
 * 功能: 封装 /dev/fb0 的打开/关闭/清屏/RGB数据上屏
 *       以后显示 RGB24 数据(图片/视频帧)不用再写 mmap/ioctl 那一套
 *
 * 编译成动态库:
 *   gcc -shared -fPIC -o liblcd_io.so lcd_io.c
 *
 * 使用:
 *   #include "lcd_io.h"
 *   FBInfo fb;
 *   fb_open(&fb);                        // 打开屏幕
 *   fb_show_rgb(&fb, rgb, w, h, 1000);   // 把 RGB 数据显示上去
 *   fb_close(&fb);                       // 关闭屏幕
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include "lcd_io.h"

/* ==================================================================
 * 打开屏幕: /dev/fb0 + mmap 显存
 * ================================================================== */
int fb_open(FBInfo *info)
{
    info->fd = open("/dev/fb0", O_RDWR);
    if (info->fd < 0) {
        perror("open /dev/fb0");
        return -1;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("ioctl FBIOGET_VSCREENINFO");
        close(info->fd);
        return -1;
    }

    info->width       = vinfo.xres;
    info->height      = vinfo.yres;
    info->bpp         = vinfo.bits_per_pixel;
    info->line_len    = vinfo.xres * (info->bpp / 8);
    info->screen_size = info->line_len * vinfo.yres;

    printf("[LCD] 屏幕: %dx%d, %dbpp\n", info->width, info->height, info->bpp);

    info->fb = mmap(NULL, info->screen_size,
                    PROT_READ | PROT_WRITE, MAP_SHARED, info->fd, 0);
    if (info->fb == MAP_FAILED) {
        perror("mmap");
        close(info->fd);
        return -1;
    }
    return 0;
}

/* ==================================================================
 * 关闭屏幕 / 清屏
 * ================================================================== */
void fb_close(FBInfo *info)
{
    if (info->fb) {
        munmap(info->fb, info->screen_size);
        info->fb = NULL;
    }
    if (info->fd >= 0) {
        close(info->fd);
        info->fd = -1;
    }
}

void fb_clear(FBInfo *info)
{
    memset(info->fb, 0, info->screen_size);
}

/* 把一个像素(RGB24)写入屏幕坐标 (x, y), 支持 16/32 bpp */
static inline void fb_put_pixel(FBInfo *info, int x, int y,
                                uint8_t r, uint8_t g, uint8_t b)
{
    int pos = y * info->width + x;
    if (info->bpp == 32) {   /* ARGB8888 */
        ((uint32_t *)info->fb)[pos] = (0xFFu << 24) | (r << 16) | (g << 8) | b;
    } else {                 /* RGB565 */
        ((uint16_t *)info->fb)[pos] =
            ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }
}

/* ==================================================================
 * RGB24 → 屏幕 (居中, 可缩放)
 * scale: 1000=原尺寸; <1000 缩小; >1000 放大 (基于1000的千分比)
 * ================================================================== */
int fb_show_rgb(FBInfo *info, const char *rgb, int w, int h, int scale)
{
    if (!rgb || w <= 0 || h <= 0) {
        fprintf(stderr, "[fb_show_rgb] 图片数据为空!\n");
        return -1;
    }
    if (scale <= 0) scale = 1000;

    int disp_w = w * scale / 1000;
    int disp_h = h * scale / 1000;
    int off_x  = (info->width  - disp_w) / 2;   /* 居中偏移 */
    int off_y  = (info->height - disp_h) / 2;

    /* 图大 → 居中裁剪; 图小 → 居中显示, 四周黑色 */
    for (int y = 0; y < info->height; y++) {
        for (int x = 0; x < info->width; x++) {
            /* 屏幕坐标 → 图片坐标 */
            int sx = (x - off_x) * 1000 / scale;
            int sy = (y - off_y) * 1000 / scale;

            /* 超出图片范围 → 黑色 */
            if (sx < 0 || sx >= w || sy < 0 || sy >= h) {
                fb_put_pixel(info, x, y, 0, 0, 0);
                continue;
            }
            const char *p = rgb + ((long)sy * w + sx) * 3;
            fb_put_pixel(info, x, y, p[0], p[1], p[2]);
        }
    }
    return 0;
}
