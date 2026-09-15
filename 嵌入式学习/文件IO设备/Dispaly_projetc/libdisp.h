/* libdisp.h - 显示库（库层）
 *
 * 职责：把"往屏幕上画东西"封装成简单接口，
 *      调用者不用关心 BMP/JPG 的格式细节。
 * 依赖：jpeg_display.c（jpg2rgb / fb_show_image）
 * 未来：这层可以直接编成动态库 .so
 */
#ifndef __LIBDISP_H__
#define __LIBDISP_H__

#include "jpeg_display.h"   /* FBInfo */

/* 清屏为黑色（黑屏过渡） */
void disp_clear(FBInfo *fb);

/*
 * 显示一张图片文件（自动识别 .bmp / .jpg / .jpeg）
 * scale: 1000 = 原尺寸（缩略图可传更小值）
 * 返回 0 成功，-1 失败
 */
int disp_image_file(FBInfo *fb, const char *path, int scale);

/*
 * 等比缩放铺满屏幕内（fit）并居中显示
 * 自动按屏幕分辨率计算缩放比例，大图浏览用
 * 返回 0 成功，-1 失败
 */
int disp_image_fit(FBInfo *fb, const char *path);

/*
 * 在屏幕上画 8x16 ASCII 文字（白色）
 * x, y: 左上角坐标；返回占用宽度（像素）
 */
int disp_text(FBInfo *fb, int x, int y, const char *s);

/* 画实心矩形（按钮高亮、列表背景用） */
void disp_fill_rect(FBInfo *fb, int x1, int y1, int x2, int y2,
                    uint8_t r, uint8_t g, uint8_t b);

/* 画实心圆点（页面指示器 dots 用） */
void disp_dot(FBInfo *fb, int cx, int cy, int radius,
              uint8_t r, uint8_t g, uint8_t b);

#endif
