#ifndef JPEG_DISPLAY_H
#define JPEG_DISPLAY_H

#include <stdint.h>

/* ========== 解码后的图片信息 ========== */
struct imageInfo
{
    int width;      /* 图片宽(像素) */
    int height;     /* 图片高(像素) */
    int bpp;        /* 色深: 24 = RGB24 */
    char *rgb;      /* RGB24 数据, 每像素3字节 (R,G,B), 用完要 free */
};

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
 * 函数1: JPEG文件 → RGB24
 *
 * @param jpgfile  图片文件路径
 *                 - 例如 "/root/1.jpg" 或 "1.jpg"
 *                 - 只读, 函数内部自己打开/读取/关闭, 调用者不用管
 * @param info     输出参数(函数把结果写进这个结构体)
 *                 - 函数成功后 info 里会有:
 *                     info.width   图片宽(像素)
 *                     info.height  图片高(像素)
 *                     info.bpp     色深(24=RGB24)
 *                     info.rgb     解码后的 RGB24 数据指针
 *                 调用者用完必须 free(info.rgb)
 * @return 0 = 成功, -1 = 失败
 *         失败原因已打印到屏幕, 常见: 文件不存在/不是jpg/内存不足
 */
int jpg2rgb(const char *jpgfile, struct imageInfo *info);

/*
 * 函数2: RGB24 → 显示到屏幕(居中, 可缩放)
 *
 * @param fb     屏幕信息, 必须先调用 fb_open(&fb) 得到
 *               - 显示内容会画在这块屏幕上
 *               - 函数内会读取 fb->width/height/bpp/显存地址
 * @param info   要显示的图片(jpg2rgb 的解码结果)
 *               - 只要 info 里 rgb 有效, 传指针即可
 * @param scale  缩放比例, 基于 1000 的千分比
 *               - 1000 = 原尺寸 100%
 *               - 500  = 缩小一半
 *               - 2000 = 放大一倍
 *               - 传 0 或负数会当成 1000
 * @return 0 = 成功, -1 = 失败(如图片数据为空)
 */
int fb_show_image(FBInfo *fb, const struct imageInfo *info, int scale);

/*
 * 辅助: 打开屏幕
 *
 * @param info  屏幕信息结构体(调用者定义, 如 FBInfo fb;)
 *              - 成功后填入: fd / fb(显存地址) / width / height / bpp 等
 *              - 之后传给 fb_show_image / fb_clear 用
 * @return 0 = 成功, -1 = 失败(如 /dev/fb0 不存在或 mmap 失败)
 */
int  fb_open(FBInfo *info);

/*
 * 辅助: 关闭屏幕
 *
 * @param info  之前 fb_open 得到的屏幕信息
 *              - 释放 mmap 的显存映射, 关闭 /dev/fb0
 *              - 显示完图片最后调用, 否则程序退出会有残留
 */
void fb_close(FBInfo *info);

/*
 * 辅助: 清屏为黑色
 *
 * @param info  屏幕信息
 *              - 显示新图片前调用, 把上一张图擦掉
 */
void fb_clear(FBInfo *info);

#endif /* JPEG_DISPLAY_H */
