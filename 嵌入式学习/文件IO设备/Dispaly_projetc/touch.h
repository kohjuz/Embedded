/* touch.h - 触摸屏封装（库层） */
#ifndef __TOUCH_H__
#define __TOUCH_H__

/* 打开触摸屏设备，成功返回 0 */
int touch_open(const char *dev);

/*
 * 读取一次触摸事件（阻塞至有事件或超时）
 * timeout_ms : 等待超时（毫秒），-1 = 永远等待
 * 返回:
 *   1 = 点击      (x, y 有效)
 *   2 = 滑动      (x, y 起点, x2, y2 终点)
 *   0 = 超时（无触摸）
 *  -1 = 出错
 */
int touch_read(int timeout_ms, int *x, int *y, int *x2, int *y2);

#endif
