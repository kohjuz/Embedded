#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 打开液晶屏文件
    int lcd = open("/dev/fb0", O_RDWR);

    // 给LCD设备映射一块内存（或称显存）
    char *p = mmap(NULL, 1024*600*4, PROT_WRITE,
                   MAP_SHARED, lcd, 0);

    // 通过映射内存，将LCD屏幕的每一个像素点涂成红色
    int red = 0x00FF0000;

    for(int i=0; i<1024*600; i++)
       memcpy(p+i*4, &red, 4);
      
    // 解除映射
    munmap(p, 1024*600*4);
    return 0;
}