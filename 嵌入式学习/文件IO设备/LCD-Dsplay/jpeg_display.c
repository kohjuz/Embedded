#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "jpeglib.h"
#include "jpeg_display.h"

/* ==================================================================
 * 函数1: JPEG文件 → RGB24
 * 内部自动完成: 读文件 → jpg解码 → 得到 RGB24 数据
 * ================================================================== */
int jpg2rgb(const char *jpgfile, struct imageInfo *info)
{
    /* 1. 读取 jpg 文件到内存 */
    int fd = open(jpgfile, O_RDONLY);
    if (fd < 0) {
        perror("open jpg");
        return -1;
    }
    int jpgsize = lseek(fd, 0, SEEK_END);
    if (jpgsize <= 0) {
        fprintf(stderr, "jpg 文件为空!\n");
        close(fd);
        return -1;
    }
    char *jpgdata = malloc(jpgsize);
    lseek(fd, 0, SEEK_SET);
    int n = 0;
    while (n < jpgsize) {
        int m = read(fd, jpgdata + n, jpgsize - n);
        if (m <= 0) break;
        n += m;
    }
    close(fd);

    /* 2. 用 libjpeg 把 jpg 数据解码成 RGB24 */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (unsigned char *)jpgdata, jpgsize);

    if (!jpeg_read_header(&cinfo, true)) {
        fprintf(stderr, "jpeg_read_header failed: %s\n", strerror(errno));
        jpeg_destroy_decompress(&cinfo);
        free(jpgdata);
        return -1;
    }
    jpeg_start_decompress(&cinfo);

    info->width  = cinfo.output_width;
    info->height = cinfo.output_height;
    info->bpp    = cinfo.output_components * 8;   /* 24 = RGB24 */

    unsigned long linesize = cinfo.output_width * cinfo.output_components;
    info->rgb = calloc(1, linesize * cinfo.output_height);
    if (!info->rgb) {
        jpeg_destroy_decompress(&cinfo);
        free(jpgdata);
        return -1;
    }

    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *buffer_array[1];
        buffer_array[0] = (unsigned char *)info->rgb
                        + cinfo.output_scanline * linesize;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(jpgdata);

    printf("[解码] %s: %dx%d, %dbpp\n",
           jpgfile, info->width, info->height, info->bpp);
    return 0;
}

/* ==================================================================
 * Framebuffer 操作
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

    info->width  = vinfo.xres;
    info->height = vinfo.yres;
    info->bpp    = vinfo.bits_per_pixel;
    info->line_len = vinfo.xres * (info->bpp / 8);
    info->screen_size = info->line_len * vinfo.yres;

    printf("[FB] 屏幕: %dx%d, %dbpp\n", info->width, info->height, info->bpp);

    info->fb = mmap(NULL, info->screen_size,
                    PROT_READ | PROT_WRITE, MAP_SHARED, info->fd, 0);
    if (info->fb == MAP_FAILED) {
        perror("mmap");
        close(info->fd);
        return -1;
    }
    return 0;
}

void fb_close(FBInfo *info)
{
    munmap(info->fb, info->screen_size);
    close(info->fd);
}

void fb_clear(FBInfo *info)
{
    memset(info->fb, 0, info->screen_size);
}

/* 把一个像素(RGB24)写入屏幕坐标 (x, y) */
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
 * 函数2: RGB24 → 显示到屏幕(居中)
 * scale: 1000 = 原尺寸; <1000 缩小; >1000 放大 (基于1000的千分比)
 * ================================================================== */
int fb_show_image(FBInfo *info, const struct imageInfo *img, int scale)
{
    if (!img->rgb) {
        fprintf(stderr, "图片数据为空!\n");
        return -1;
    }
    if (scale <= 0) scale = 1000;

    int disp_w = img->width * scale / 1000;
    int disp_h = img->height * scale / 1000;
    int off_x  = (info->width  - disp_w) / 2;   /* 居中偏移 */
    int off_y  = (info->height - disp_h) / 2;

    printf("[显示] 图片 %dx%d -> 显示 %dx%d, 居中偏移(%d,%d)\n",
           img->width, img->height, disp_w, disp_h, off_x, off_y);

    for (int y = 0; y < info->height; y++) {
        for (int x = 0; x < info->width; x++) {
            /* 屏幕坐标 → 图片坐标 */
            int sx = (x - off_x) * 1000 / scale;
            int sy = (y - off_y) * 1000 / scale;

            /* 超出图片范围 → 黑色 */
            if (sx < 0 || sx >= img->width || sy < 0 || sy >= img->height) {
                fb_put_pixel(info, x, y, 0, 0, 0);
                continue;
            }
            const char *p = img->rgb + (sy * img->width + sx) * 3;
            fb_put_pixel(info, x, y, p[0], p[1], p[2]);
        }
    }
    return 0;
}
