#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "lcd.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void show_image(struct lcdinfo *LCD, const char *filename)
{
    int w, h, n;

    // 统一按 RGBA 4 通道加载，简化后续写入逻辑
    unsigned char *data = stbi_load(filename, &w, &h, &n, 4);
    if(data == NULL)
    {
        fprintf(stderr, "stbi_load [%s] failed\n", filename);
        return;
    }

    printf("PNG width:  %d\n", w);
    printf("PNG height: %d\n", h);
    printf("PNG channels(orig): %d\n", n);

    // 等比缩放 + 居中
    // scale 取宽高比例中较小者，保证整张图完整显示、不裁边；
    // 上限 1.0：大图缩小，小图保持原大小只居中（不放大糊掉）
    double scale = fmin(1.0, fmin((double)LCD->width / w, (double)LCD->height / h));
    int dst_w = (int)(w * scale);
    int dst_h = (int)(h * scale);
    if(dst_w < 1) dst_w = 1;
    if(dst_h < 1) dst_h = 1;
    int off_x = (LCD->width - dst_w) / 2;
    int off_y = (LCD->height - dst_h) / 2;

    // 最近邻采样：目标像素 (di,dj) 直接取原图最近的点 (sx,sy)
    // 把 RGBA 数据写入 framebuffer
    // 根据同项目 JPG 处理可知：LCD 显存每像素 4 字节，低 3 字节为 BGR 顺序
    for(int dj = 0; dj < dst_h; dj++)
    {
        int sy = dj * h / dst_h;
        for(int di = 0; di < dst_w; di++)
        {
            int sx = di * w / dst_w;

            unsigned char *src = data + 4 * (sx + w * sy);
            unsigned char *dst = LCD->fbmem + (off_y+dj)*LCD->pitch + (off_x+di)*4;

            dst[0] = src[2]; // B
            dst[1] = src[1]; // G
            dst[2] = src[0]; // R
            dst[3] = src[3]; // A
        }
    }

    stbi_image_free(data);
}
