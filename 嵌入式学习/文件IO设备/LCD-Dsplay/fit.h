#ifndef FIT_H
#define FIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#include "bmp.h"

/* ============================================================
 *  fit 对齐库
 *  提供多种BMP图像在LCD上的对齐/适配显示方式：
 *    FIT_TOP_LEFT   : 左上角对齐（原始大小，超出裁剪）
 *    FIT_CENTER     : 居中对齐（原始大小，超出裁剪）
 *    FIT_STRETCH    : 拉伸铺满（不保留宽高比）
 *    FIT_CONTAIN    : 等比缩放并完整显示（居中，可能有黑边）
 *    FIT_COVER      : 等比缩放铺满屏幕（居中，可能裁剪）
 * ============================================================ */

typedef enum
{
    FIT_TOP_LEFT = 0,
    FIT_CENTER,
    FIT_STRETCH,
    FIT_CONTAIN,
    FIT_COVER,
} fit_mode_t;

/* LCD资源描述 */
typedef struct
{
    int       fd;
    char     *fb;       /* mmap后的显存首地址 */
    int       width;    /* 屏幕宽 */
    int       height;   /* 屏幕高 */
    int       bpp;      /* 色深(位) */
    int       line_size;/* 一行字节数 */
    int       size;     /* 显存总字节 */
} lcd_t;

/* BMP解码后的RGB数据描述 */
typedef struct
{
    char     *data;     /* RGB数据(含pad) */
    int       width;
    int       height;
    int       bpp;
    int       pixel;    /* 每像素字节数 */
    int       line_size;/* 一行字节数(含pad) */
} bmp_t;

/* ---------- LCD 相关 ---------- */
int  lcd_open(lcd_t *lcd, const char *dev);
void lcd_close(lcd_t *lcd);

/* ---------- BMP 解码 ---------- */
/* 从已打开的fd读取BMP，成功返回0，失败返回-1 */
int  bmp_load_fd(bmp_t *bmp, int fd);
void bmp_free(bmp_t *bmp);

/* ---------- 显示接口 ---------- */
/* 以指定模式将bmp显示到lcd */
int  fit_draw(const lcd_t *lcd, const bmp_t *bmp, fit_mode_t mode);

/* 便捷接口：直接给bmp文件路径 */
int  fit_show_file(const char *bmp_path, fit_mode_t mode);

/* ============================================================
 *  扩展功能
 * ============================================================ */

/* 在LCD指定坐标(x,y)处开始绘制bmp（原始大小，超出裁剪）
 * 用法: fit_draw_at(&lcd, &bmp, 100, 50);
 *       将图片左上角放在LCD的(100,50)，超出屏幕部分自动裁剪 */
int  fit_draw_at(const lcd_t *lcd, const bmp_t *bmp, int x, int y);

/* 将bmp缩放ratio倍后绘制到LCD（居中）
 * 用法: fit_draw_scale(&lcd, &bmp, 0.5);   // 缩小一半居中显示
 *       fit_draw_scale(&lcd, &bmp, 2.0);   // 放大两倍居中显示
 *       ratio > 1 放大，0 < ratio < 1 缩小 */
int  fit_draw_scale(const lcd_t *lcd, const bmp_t *bmp, double ratio);

/* 左右镜像翻转后绘制到LCD（可配合mode指定缩放策略）
 * 用法: fit_draw_mirror(&lcd, &bmp, FIT_CENTER);  // 原始大小居中镜像
 *       fit_draw_mirror(&lcd, &bmp, FIT_STRETCH);  // 镜像后拉伸铺满
 *       镜像=左右翻转，相当于照镜子效果 */
int  fit_draw_mirror(const lcd_t *lcd, const bmp_t *bmp, fit_mode_t mode);

/* 旋转指定角度后绘制到LCD（居中）
 * angle 仅支持 0 / 90 / 180 / 270
 * 用法: fit_draw_rotate(&lcd, &bmp, 90);    // 顺时针旋转90度
 *       fit_draw_rotate(&lcd, &bmp, 180);   // 旋转180度(上下颠倒)
 *       fit_draw_rotate(&lcd, &bmp, 270);   // 顺时针旋转270度
 * 注意: 旋转90/270度后图片宽高会互换 */
int  fit_draw_rotate(const lcd_t *lcd, const bmp_t *bmp, int angle);

/* 用RGB颜色清屏（整个LCD填充指定颜色）
 * 用法: fit_clear(&lcd, 0, 0, 0);         // 清成黑色
 *       fit_clear(&lcd, 255, 255, 255);    // 清成白色
 *       fit_clear(&lcd, 255, 0, 0);       // 清成红色
 * 参数: r/g/b 取值范围 0~255 */
void fit_clear(const lcd_t *lcd, unsigned char r,
                                 unsigned char g,
                                 unsigned char b);

/* 用RGB颜色填充LCD上的矩形区域
 * 用法: fit_fill_rect(&lcd, 0, 0, 100, 50, 255, 0, 0);  // 左上角100x50红色块
 *       fit_fill_rect(&lcd, 50, 50, 200, 100, 0, 255, 0); // 绿色矩形
 * 参数: (x,y) 矩形左上角坐标, w/h 宽高, r/g/b 颜色(0~255)
 *       越界部分自动裁剪 */
void fit_fill_rect(const lcd_t *lcd, int x, int y, int w, int h,
                   unsigned char r, unsigned char g, unsigned char b);

/* 将bmp绘制到LCD上的指定矩形区域内（拉伸至该区域大小）
 * 用法: fit_draw_rect(&lcd, &bmp, 0, 0, 200, 200);    // 拉伸到左上角200x200
 *       fit_draw_rect(&lcd, &bmp, 50, 50, 300, 200);  // 拉伸到指定区域
 * 参数: (x,y) 目标区域左上角, w/h 目标区域宽高
 *       图片会被拉伸/压缩到正好填充该区域 */
int  fit_draw_rect(const lcd_t *lcd, const bmp_t *bmp,
                   int x, int y, int w, int h);

#endif
