/* libdisp.c - 显示库实现（库层）
 *
 * 包含：
 *   1. BMP 文件解析 + 显示（支持缩放、居中）
 *   2. 自动识别 bmp/jpg 并显示
 *   3. 8x16 ASCII 文字渲染
 *   4. 矩形 / 圆点（UI 元素）
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include "libdisp.h"
#include "font8x16.h"

/* ========== BMP 文件头结构（参考 photo.c，独立重写） ========== */
#pragma pack(push, 1)
struct bmp_header {
    uint16_t type;      /* 'BM' */
    uint32_t size;      /* 文件大小 */
    uint16_t r1, r2;
    uint32_t offbits;   /* 像素数据偏移 */
};
struct bmp_info {
    uint32_t size;
    int32_t  width, height;
    uint16_t planes;
    uint16_t bit_count;     /* 色深：24/32 */
    uint32_t compression;
    uint32_t size_img;
    int32_t  x_pel, y_pel;
    uint32_t clrused, clr_imp;
};
#pragma pack(pop)

/* ========== 画一个像素（按屏幕色深） ========== */
static void disp_pixel(FBInfo *fb, int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= fb->width || y < 0 || y >= fb->height)
        return;
    int pos = y * fb->width + x;
    if (fb->bpp == 32) {
        ((uint32_t *)fb->fb)[pos] = (0xFFu << 24) | (r << 16) | (g << 8) | b;
    } else {
        ((uint16_t *)fb->fb)[pos] =
            ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }
}

void disp_clear(FBInfo *fb)
{
    memset(fb->fb, 0, fb->screen_size);
}

/* ========== BMP 显示（支持缩放、居中、越界裁剪） ========== */
static int bmp_show(FBInfo *fb, const char *path, int scale)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("bmp open");
        return -1;
    }

    struct bmp_header hd;
    struct bmp_info   bi;
    if (read(fd, &hd, sizeof(hd)) != (ssize_t)sizeof(hd) ||
        read(fd, &bi, sizeof(bi)) != (ssize_t)sizeof(bi)) {
        perror("bmp read header");
        close(fd);
        return -1;
    }
    if (hd.type != 0x4D42) {        /* 'BM' */
        fprintf(stderr, "not a bmp: %s\n", path);
        close(fd);
        return -1;
    }

    int w = bi.width, h = bi.height;
    int pixel = bi.bit_count / 8;

    /* 每行补位：BMP 行字节数必须是 4 的倍数 */
    int pad = (4 - (w * pixel % 4)) % 4;
    int line_size = w * pixel + pad;

    /* 读取全部像素数据 */
    char *rgb = malloc((size_t)line_size * h);
    if (!rgb) {
        close(fd);
        return -1;
    }
    size_t got = 0, need = (size_t)line_size * h;
    while (got < need) {
        ssize_t n = read(fd, rgb + got, need - got);
        if (n <= 0) break;
        got += n;
    }
    close(fd);

    /* 缩放目标尺寸 */
    if (scale <= 0) scale = 1000;
    int dw = w * scale / 1000;
    int dh = h * scale / 1000;
    int off_x = (fb->width  - dw) / 2;
    int off_y = (fb->height - dh) / 2;

    for (int j = 0; j < dh; j++) {
        int y_lcd = off_y + j;
        if (y_lcd < 0 || y_lcd >= fb->height)
            continue;
        /* 目标行对应的源行（BMP 存储是倒序：第一行是图片最底行） */
        int src_y = j * 1000 / scale;
        int bmp_row = h - 1 - src_y;
        int bmp_off = line_size * bmp_row;

        for (int i = 0; i < dw; i++) {
            int x_lcd = off_x + i;
            if (x_lcd < 0 || x_lcd >= fb->width)
                continue;
            int src_x = i * 1000 / scale;
            const char *p = rgb + bmp_off + src_x * pixel;
            disp_pixel(fb, x_lcd, y_lcd, p[2], p[1], p[0]);  /* BGR -> RGB */
        }
    }

    free(rgb);
    return 0;
}

/* ========== 自动识别 bmp / jpg 并显示 ========== */
int disp_image_file(FBInfo *fb, const char *path, int scale)
{
    const char *ext = strrchr(path, '.');
    if (!ext) {
        fprintf(stderr, "no ext: %s\n", path);
        return -1;
    }
    if (strcasecmp(ext, ".bmp") == 0)
        return bmp_show(fb, path, scale);

    if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0) {
        struct imageInfo img;
        if (jpg2rgb(path, &img) != 0)
            return -1;
        fb_show_image(fb, &img, scale);
        free(img.rgb);
        return 0;
    }
    fprintf(stderr, "unsupported: %s\n", path);
    return -1;
}

/* ========== 只读 BMP 尺寸（fit 计算用） ========== */
static int bmp_size(const char *path, int *w, int *h)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return -1;
    struct bmp_header hd;
    struct bmp_info   bi;
    ssize_t n1 = read(fd, &hd, sizeof(hd));
    ssize_t n2 = read(fd, &bi, sizeof(bi));
    close(fd);
    if (n1 != (ssize_t)sizeof(hd) || n2 != (ssize_t)sizeof(bi))
        return -1;
    if (hd.type != 0x4D42)
        return -1;
    *w = bi.width;
    *h = bi.height;
    return 0;
}

/* ========== 等比缩放铺满屏幕（fit） ========== */
int disp_image_fit(FBInfo *fb, const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext)
        return -1;

    if (strcasecmp(ext, ".bmp") == 0) {
        int w, h;
        if (bmp_size(path, &w, &h) != 0)
            return -1;
        int s = fb->width  * 1000 / w;
        int t = fb->height * 1000 / h;
        int scale = (s < t) ? s : t;
        if (scale < 1) scale = 1;
        return bmp_show(fb, path, scale);
    }

    if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0) {
        struct imageInfo img;
        if (jpg2rgb(path, &img) != 0)
            return -1;
        int s = fb->width  * 1000 / img.width;
        int t = fb->height * 1000 / img.height;
        int scale = (s < t) ? s : t;
        if (scale < 1) scale = 1;
        fb_show_image(fb, &img, scale);
        free(img.rgb);
        return 0;
    }
    return -1;
}

/* ========== 8x16 ASCII 文字 ========== */
int disp_text(FBInfo *fb, int x, int y, const char *s)
{
    int cx = x;
    for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (c < 32 || c > 126)
            c = '?';                /* 非 ASCII 画成问号 */
        const unsigned char *g = font8x16[c - 32];
        for (int row = 0; row < 16; row++) {
            if (g[row] == 0)
                continue;
            for (int col = 0; col < 8; col++) {
                if (g[row] & (0x80 >> col))
                    disp_pixel(fb, cx + col, y + row, 255, 255, 255);
            }
        }
        cx += 8;
    }
    return cx - x;
}

/* ========== 实心矩形 ========== */
void disp_fill_rect(FBInfo *fb, int x1, int y1, int x2, int y2,
                    uint8_t r, uint8_t g, uint8_t b)
{
    for (int yy = y1; yy <= y2; yy++)
        for (int xx = x1; xx <= x2; xx++)
            disp_pixel(fb, xx, yy, r, g, b);
}

/* ========== 实心圆点 ========== */
void disp_dot(FBInfo *fb, int cx, int cy, int radius,
              uint8_t r, uint8_t g, uint8_t b)
{
    for (int yy = cy - radius; yy <= cy + radius; yy++)
        for (int xx = cx - radius; xx <= cx + radius; xx++) {
            int dx = xx - cx, dy = yy - cy;
            if (dx * dx + dy * dy <= radius * radius)
                disp_pixel(fb, xx, yy, r, g, b);
        }
}
