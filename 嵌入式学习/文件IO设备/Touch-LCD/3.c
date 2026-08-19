#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>


#include "jpeg_display.h"

#define TOUCH_DEV   "/dev/input/event6"
#define PALETTE_H   60          /* 底部调色板高度 */
#define LINE_THICK  2           /* 线条粗细（像素） */


static void draw_palette(void);
static void clear_canvas(void);

static FBInfo fb;
static int tp;

/* 调色板颜色 */
struct color_info {
    const char *name;
    uint8_t r, g, b;
};

static struct color_info colors[] = {
    {"红",   255, 0,   0},
    {"绿",   0,   255, 0},
    {"蓝",   0,   0,   255},
    {"黄",   255, 255, 0},
    {"品红", 255, 0,   255},
    {"青",   0,   255, 255},
    {"黑",   0,   0,   0},
    {"白",   255, 255, 255}, /* 兼作橡皮 */
};
#define COLOR_NUM   (sizeof(colors) / sizeof(colors[0]))

static int cur_color = 0;       /* 当前选中的颜色索引 */

/* 触摸屏物理范围 -> 屏幕坐标 */
static int touch_min_x = 0, touch_max_x = 0;
static int touch_min_y = 0, touch_max_y = 0;

static int map_x(int v)
{
    if (touch_max_x <= touch_min_x)
        return v;
    long x = (long)(v - touch_min_x) * (fb.width  - 1) / (touch_max_x - touch_min_x);
    if (x < 0) x = 0;
    if (x >= fb.width) x = fb.width - 1;
    return (int)x;
}

static int map_y(int v)
{
    if (touch_max_y <= touch_min_y)
        return v;
    long y = (long)(v - touch_min_y) * (fb.height - 1) / (touch_max_y - touch_min_y);
    if (y < 0) y = 0;
    if (y >= fb.height) y = fb.height - 1;
    return (int)y;
}

/* 读取触摸屏的 ABS 范围 */
static int get_touch_range(void)
{
    struct input_absinfo abs;

    if (ioctl(tp, EVIOCGABS(ABS_X), &abs) < 0) {
        perror("ioctl EVIOCGABS ABS_X");
        return -1;
    }
    touch_min_x = abs.minimum;
    touch_max_x = abs.maximum;

    if (ioctl(tp, EVIOCGABS(ABS_Y), &abs) < 0) {
        perror("ioctl EVIOCGABS ABS_Y");
        return -1;
    }
    touch_min_y = abs.minimum;
    touch_max_y = abs.maximum;

    printf("[触摸] X范围: %d~%d, Y范围: %d~%d\n",
           touch_min_x, touch_max_x, touch_min_y, touch_max_y);
    return 0;
}

/* 画一个像素 */
static inline void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= fb.width || y < 0 || y >= fb.height)
        return;

    if (fb.bpp == 32) {
        ((uint32_t *)fb.fb)[y * fb.width + x] =
            (0xFFu << 24) | (r << 16) | (g << 8) | b;
    } else { /* 默认 RGB565 */
        ((uint16_t *)fb.fb)[y * fb.width + x] =
            ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }
}

/* 画实心圆（用于加粗线条） */
static void fill_circle(int cx, int cy, int r, uint8_t red, uint8_t gr, uint8_t bl)
{
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r + r)
                put_pixel(cx + x, cy + y, red, gr, bl);
        }
    }
}

/* Bresenham 直线，两端用圆点加粗 */
static void draw_line(int x0, int y0, int x1, int y1,
                      uint8_t r, uint8_t g, uint8_t b)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        fill_circle(x0, y0, LINE_THICK / 2, r, g, b);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/* 填充矩形 */
static void fill_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    for (int yy = y; yy < y + h && yy < fb.height; yy++) {
        for (int xx = x; xx < x + w && xx < fb.width; xx++) {
            put_pixel(xx, yy, r, g, b);
        }
    }
}
static void clear_canvas(void)
{
    //只清除绘画区，保留底部调色板
    int canvas_h = fb.height - PALETTE_H;
    for(int y=0;y<canvas_h;y++)
    {
        for(int x=0;x<fb.width;x++)
        {
            put_pixel(x,y,255,255,255);
        }
    }
    draw_palette();
}


/* 绘制底部调色板 */
static void draw_palette(void)
{
    int btn_w = fb.width / COLOR_NUM;
    int y0 = fb.height - PALETTE_H;

    for (int i = 0; i < COLOR_NUM; i++) {
        int x0 = i * btn_w;
        fill_rect(x0, y0, btn_w, PALETTE_H,
                  colors[i].r, colors[i].g, colors[i].b);

        /* 当前选中颜色加白色边框 */
        if (i == cur_color) {
            for (int t = 0; t < 4; t++) {
                for (int xx = x0 + t; xx < x0 + btn_w - t; xx++) {
                    put_pixel(xx, y0 + t, 255, 255, 255);
                    put_pixel(xx, y0 + PALETTE_H - 1 - t, 255, 255, 255);
                }
                for (int yy = y0 + t; yy < y0 + PALETTE_H - t; yy++) {
                    put_pixel(x0 + t, yy, 255, 255, 255);
                    put_pixel(x0 + btn_w - 1 - t, yy, 255, 255, 255);
                }
            }
        }
    }
}

/* 根据 x 坐标选择调色板颜色 */
static void pick_color(int x)
{
    int idx = x / (fb.width / COLOR_NUM);
    if (idx < 0) idx = 0;
    if (idx >= (int)COLOR_NUM) idx = COLOR_NUM - 1;

    cur_color = idx;

    /* 点击白色色块：一键清空画布 */
    if(idx == 7)
    {
        printf("执行清空画布\n");
        clear_canvas();
    }
    else
    {
        printf("选择颜色: %s (%d,%d,%d)\n",
               colors[idx].name, colors[idx].r, colors[idx].g, colors[idx].b);
        draw_palette();
    }
}


int main(int argc, char *argv[])
{
    const char *touch_dev = (argc > 1) ? argv[1] : TOUCH_DEV;

    /* 1. 打开 framebuffer */
    if (fb_open(&fb) != 0) {
        fprintf(stderr, "打开屏幕失败\n");
        return -1;
    }

    /* 2. 清屏为白色 */
    for (int i = 0; i < fb.width * fb.height; i++) {
        if (fb.bpp == 32)
            ((uint32_t *)fb.fb)[i] = 0xFFFFFFFF;
        else
            ((uint16_t *)fb.fb)[i] = 0xFFFF;
    }

    /* 3. 打开触摸屏 */
    tp = open(touch_dev, O_RDWR);
    if (tp < 0) {
        perror("open touch");
        fb_close(&fb);
        return -1;
    }

    if (get_touch_range() != 0) {
        close(tp);
        fb_close(&fb);
        return -1;
    }

    /* 4. 绘制调色板 */
    draw_palette();

    /* 5. 主循环：读取触摸事件并绘制 */
    struct input_event buf;
    bool pressed = false;
    bool last_valid = false;
    bool x_new = false, y_new = false;
    int cur_x = 0, cur_y = 0;
    int last_x = 0, last_y = 0;

    printf("画板已启动，用手指在屏幕上方绘画，点击底部色块换色。\n");

    while (1) {
        if (read(tp, &buf, sizeof(buf)) != (int)sizeof(buf))
            continue;

        if (buf.type == EV_ABS) {
            if (buf.code == ABS_X) {
                cur_x = map_x(buf.value);
                x_new = true;
            } else if (buf.code == ABS_Y) {
                cur_y = map_y(buf.value);
                y_new = true;
            }
        } else if (buf.type == EV_KEY && buf.code == BTN_TOUCH) {
            if (buf.value == 1) {           /* 按下 */
                pressed = true;
                last_valid = false;
                x_new = y_new = false;
            } else {                         /* 抬起 */
                pressed = false;
                /* 在调色板区域抬起 => 换色 */
                if (cur_y >= fb.height - PALETTE_H)
                    pick_color(cur_x);
                last_valid = false;
                x_new = y_new = false;
            }
        } else if (buf.type == EV_SYN && pressed && x_new && y_new) {
            int draw_y = fb.height - PALETTE_H;

            /* 起点和终点都在画板区才绘制 */
            if (last_y < draw_y && cur_y < draw_y) {
                if (last_valid) {
                    draw_line(last_x, last_y, cur_x, cur_y,
                              colors[cur_color].r,
                              colors[cur_color].g,
                              colors[cur_color].b);
                } else {
                    fill_circle(cur_x, cur_y, LINE_THICK / 2,
                                colors[cur_color].r,
                                colors[cur_color].g,
                                colors[cur_color].b);
                }
            }

            last_x = cur_x;
            last_y = cur_y;
            last_valid = true;
            x_new = y_new = false;
        }
    }

    fb_close(&fb);
    close(tp);
    return 0;
}
