#ifndef LCD_IO_H
#define LCD_IO_H

#include <stdint.h>

/* ========== 屏幕(Framebuffer)信息 ========== */
typedef struct {
    int fd;                 /* /dev/fb0 文件描述符 */
    void *fb;               /* mmap 后的显存首地址 */
    int width, height;      /* 屏幕分辨率 */
    int bpp;                /* 每像素位数: 16 / 32 */
    int line_len;           /* 每行字节数 */
    int screen_size;        /* 显存总字节数 */
} FBInfo;

/*
 * 打开屏幕: 打开 /dev/fb0 并 mmap 显存
 * @param info  调用者定义的结构体(如 FBInfo fb;), 成功后自动填好屏幕信息
 * @return 0 = 成功, -1 = 失败(打开失败/ioctl失败/mmap失败)
 */
int  fb_open(FBInfo *info);

/*
 * 关闭屏幕: 释放 mmap 映射, 关闭 /dev/fb0
 */
void fb_close(FBInfo *info);

/*
 * 清屏为黑色
 */
void fb_clear(FBInfo *info);

/*
 * 把 RGB24 数据显示到屏幕 (居中, 可缩放)
 *
 * @param info  屏幕信息, 必须先 fb_open()
 * @param rgb   RGB24 数据 (每像素3字节, 顺序: R,G,B)
 * @param w,h   图片宽高(像素)
 * @param scale 缩放千分比: 1000=原尺寸, 500=缩小一半, 2000=放大一倍
 *              传 0 或负数 = 原尺寸
 *              图片比屏幕小 → 居中显示, 四周黑色
 *              图片比屏幕大 → 居中裁剪, 只显示屏幕内部分
 * @return 0 = 成功, -1 = 失败(数据为空)
 */
int fb_show_rgb(FBInfo *info, const char *rgb, int w, int h, int scale);

#endif /* LCD_IO_H */
