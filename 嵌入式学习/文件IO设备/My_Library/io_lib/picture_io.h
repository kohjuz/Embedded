#ifndef PICTURE_IO_H
#define PICTURE_IO_H

#include <stdint.h>

/*
 * 图片信息结构体
 * 调用 get_picture() 成功后，图片的数据就存在这里
 */
typedef struct {
    int   width;    /* 图片宽度  (像素) */
    int   height;   /* 图片高度  (像素) */
    int   bpp;      /* 色深      (24 = RGB24, 每像素3字节) */
    char *rgb;      /* RGB数据   (用完必须 free !) */
} PictureInfo;

/*
 * 获取图片数据 (动态库唯一对外接口)
 *
 * 参数:
 *   filename - 图片路径, 支持 .bmp / .jpg / .jpeg
 *   info     - 传出参数, 成功后里面就是图片数据
 *
 * 返回:
 *   0  = 成功, info 里有数据了
 *  -1  = 失败 (文件不存在/格式不支持/解码错误)
 *
 * 用完记得:
 *   free(info.rgb);
 *
 * 示例:
 *   PictureInfo info;
 *   if (get_picture("1.bmp", &info) == 0) {
 *       printf("图片: %dx%d\n", info.width, info.height);
 *       // 此时 info.rgb 里就是 RGB 数据了
 *       free(info.rgb);
 *   }
 */
int get_picture(const char *filename, PictureInfo *info);

#endif /* PICTURE_IO_H */
