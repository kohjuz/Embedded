#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "lcd.h"
#include "image.h"

void show_image(struct lcdinfo *LCD, const char *bmpfile)
{
    printf("[%s][%d]\n", __FILE__, __LINE__);

    // 准备BMP图片资源
    int bmp_fd = open(bmpfile, O_RDWR);
    if (bmp_fd == -1)
    {
        perror("open bmp failed");
        exit(1);
    }

    // 获取BMP图片参数
    struct bitmap_header bmp_header;
    struct bitmap_info   bmp_info;
    bzero(&bmp_header, sizeof(bmp_header));
    bzero(&bmp_info, sizeof(bmp_info));
    read(bmp_fd, &bmp_header, sizeof(bmp_header));
    read(bmp_fd, &bmp_info, sizeof(bmp_info));

    if(bmp_info.compression != 0)
    {
        struct rgb_quad quad;
        bzero(&quad, sizeof(quad));
        read(bmp_fd, &quad, sizeof(quad));
    }

    int bmp_width = bmp_info.width;
    int bmp_height = bmp_info.height;
    int bmp_bpp = bmp_info.bit_count;

    //                 65     *  3 = 195 + pad = 196
    int bmp_pitch = bmp_width * bmp_bpp / 8; // 不含无效字节pad
    int pad = (4-bmp_pitch%4) % 4;

    int bmp_size = bmp_pitch * bmp_height;   // 不含无效字节pad

    // 1. 把整幅BMP的RGB数据读入内存
    //    注意：BMP 行是自下而上存储的（第0行是图片最底行）
    unsigned char *bmpdata = calloc(1, bmp_size);
    for(int j = 0; j < bmp_height; j++)
    {
        read(bmp_fd, bmpdata + j*bmp_pitch, bmp_pitch);
        lseek(bmp_fd, pad, SEEK_CUR); // 跳过每行末尾的pad
    }
    close(bmp_fd);

    // 2. 等比缩放 + 居中
    //    scale 取宽高比例中较小者，保证整张图完整显示、不裁边；
    //    上限 1.0：大图缩小，小图保持原大小只居中（不放大糊掉）
    double scale = fmin(1.0, fmin((double)LCD->width / bmp_width, (double)LCD->height / bmp_height));
    int dst_w = (int)(bmp_width * scale);
    int dst_h = (int)(bmp_height * scale);
    if(dst_w < 1) dst_w = 1;
    if(dst_h < 1) dst_h = 1;
    int off_x = (LCD->width - dst_w) / 2;
    int off_y = (LCD->height - dst_h) / 2;

    // 3. 最近邻采样写入LCD显存
    //    BMP 自下而上：目标行 dj(最上=0) 对应 bmp 的 (bmp_height-1-sy) 行
    for(int dj = 0; dj < dst_h; dj++)
    {
        int sy = dj * bmp_height / dst_h;          // 目标行 dj 对应 bmp 从上往下第 sy 行
        int by = bmp_height - 1 - sy;              // 转成 bmp 存储行（自下而上）
        for(int di = 0; di < dst_w; di++)
        {
            int sx = di * bmp_width / dst_w;

            unsigned char *src = bmpdata + by*bmp_pitch + sx*(bmp_bpp/8); // B,G,R(,A)
            unsigned char *dst = (unsigned char *)LCD->fbmem
                                 + (off_y+dj)*LCD->pitch + (off_x+di)*4;

            dst[0] = src[0]; // B
            dst[1] = src[1]; // G
            dst[2] = src[2]; // R
        }
    }

    free(bmpdata);
}
