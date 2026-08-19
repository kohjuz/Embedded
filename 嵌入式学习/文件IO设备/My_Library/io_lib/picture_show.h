#ifndef PICTURE_SHOW_H
#define PICTURE_SHOW_H

/*
 * 图片显示动态库: 一行代码把 BMP/JPG 显示到 LCD 屏幕
 *
 * 以后写照片浏览器/相册项目, 编译时加上两个库就能实现基本功能:
 *   gcc main.c -L. -lpicture_show -llcd_io -lpicture_io \
 *              -Wl,-rpath,'$ORIGIN' -o main
 *
 * 示例:
 *   #include "picture_show.h"
 *
 *   int main(int argc, char **argv)
 *   {
 *       photo_display(argv[1]);   // 一行显示任意格式图片, 自动判断 BMP/JPG
 *       return 0;
 *   }
 */

/*
 * 显示 BMP 图片 (原尺寸居中, 超过屏幕的部分裁剪)
 * @return 0 = 成功, -1 = 失败
 */
int bmp_display(const char *path);

/*
 * 显示 JPG 图片 (等比缩放, 铺满屏幕内)
 * @return 0 = 成功, -1 = 失败
 */
int jpg_display(const char *path);

/*
 * 一键显示图片: 根据扩展名自动选择 BMP/JPG 解码并上屏
 * (等价于原来 photo.c 里的 photo_output(), 但不用写一大段代码)
 * @return 0 = 成功, -1 = 失败(文件不存在/格式不支持/解码错误)
 */
int photo_display(const char *path);

/*
 * 按指定比例显示任意格式图片
 * @param scale 千分比: 1000=原尺寸, 500=缩小一半, 2000=放大一倍
 * @return 0 = 成功, -1 = 失败
 */
int photo_display_scaled(const char *path, int scale);

#endif /* PICTURE_SHOW_H */
