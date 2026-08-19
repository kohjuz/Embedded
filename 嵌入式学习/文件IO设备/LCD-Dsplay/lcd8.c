#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define ARGB(a,r,g,b) ((a)<<24|(r)<<16|(g)<<8|(b))

int main(int argc,char **argv)
{
    int fd =open("/dev/fb0",O_RDWR);
    if(fd<0)
    {
       perror("open");
       return -1;
    }
    struct fb_var_screeninfo var_info;
    ioctl(fd,FBIOGET_VSCREENINFO,&var_info);
    printf("var_info.xres =%d\n",var_info.xres);
    printf("var_info.yres =%d\n",var_info.yres);
    int w=var_info.xres;
    int h=var_info.yres;


    int *mp=mmap(NULL,w*h*4,PROT_WRITE,MAP_SHARED,fd,0);
    if(mp<0)
    {
       perror("mmap");
       return -1;
    }

    int color[8]=
    {
        ARGB(0,0,0,0),//透明
        ARGB(0,255,0,0),
        ARGB(0,0,255,0),
        ARGB(0,255,255,0),//黄色
        ARGB(0,0,0,255),
        ARGB(0,255,0,255),//品红色
        ARGB(0,0,255,255),//青色
        ARGB(0,255,255,255),//白色
    };

    int bw = w / 4;   // 每块宽 = 屏幕宽/4
    int bh = h / 2;   // 每块高 = 屏幕高/2

    int offset = 0;   

    while (1) 
    {
        for (int block = 0; block < 8; block++) 
        {
           
            int col = block % 4; // 0 1 2 3      
            int row = block / 4; // 0 1
            int sx = col * bw;            
            int sy = row * bh;             

        
             unsigned int c = color[(block + offset) % 8];

            // 填充这个矩形区域
            for (int y = sy; y < sy+bh; y++)
                for (int x = sx; x < sx+bw; x++)
                    mp[y * w + x] = c;
        }

        offset--;          // 逆时针：偏移量减小（颜色往"前"一个格子跑）
        if (offset < 0) offset = 7;

        usleep(500000);    // 每0.5秒流一次（改这个数调速度）
    }

    munmap(mp, w*h*4);
    close(fd);
    return 0;
}
