/*
 * show_demo.c — 演示: 用动态库显示图片
 *
 * 对比一下: 原来 photo.c 里显示一张图要手写几百行
 *           (bmp_display 120行 + jpg2rgb/fb_* 一堆)
 *           现在链接上两个库, 一行 photo_display() 就搞定
 *
 * 编译(make demo 或手动):
 *   gcc -o show_demo show_demo.c -L. -lpicture_show -llcd_io \
 *       -lpicture_io -Wl,-rpath,'$ORIGIN'
 *
 * 运行:
 *   ./show_demo 1.jpg 2.bmp 3.jpg    # 逐张显示, 每张停留2秒
 */

#include <stdio.h>
#include <unistd.h>

#include "picture_show.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("用法: %s <图片.bmp|.jpg> [...]\n", argv[0]);
        printf("示例: %s wall_b.jpg\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        printf("显示: %s\n", argv[i]);
        if (photo_display(argv[i]) != 0) {
            printf("显示失败: %s\n", argv[i]);
            continue;
        }
        sleep(2);   /* 停留2秒, 再看下一张 */
    }

    printf("全部显示完成\n");
    return 0;
}
