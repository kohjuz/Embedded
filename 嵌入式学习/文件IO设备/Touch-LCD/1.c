#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input.h>    
#include <stdbool.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>


#define ARGB(a,r,g,b) ((a)<<24|(r)<<16|(g)<<8|(b))

int color[9]=
{
    ARGB(0,0,0,0),      // 全透明
    ARGB(0,255,0,0),    // 红色
    ARGB(0,0,255,0),    // 绿色
    ARGB(0,255,255,0),  // 黄色
    ARGB(0,0,0,255),    // 蓝色
    ARGB(0,255,0,255),  // 品红(紫色)
    ARGB(0,0,255,255),  // 青色
    ARGB(0,255,255,255),// 白色
    ARGB(255,0,0,0)     // 完全不透明黑色

};

void color_lcd(int color[],int j)
{
    int fd =open("/dev/fb0",O_RDWR);
    if(fd==-1)
    {
        perror("open ");
        return;
    }
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    ioctl(fd,FBIOGET_VSCREENINFO,&vinfo);
    ioctl(fd,FBIOGET_FSCREENINFO,&finfo);
    int width = vinfo.xres;
    int height = vinfo.yres;
    int bpp = vinfo.bits_per_pixel;
    int size = width*height*bpp/8;

    unsigned int *p=mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(p==MAP_FAILED)
    {
        perror("mmap");
        return;
    }
    
        for(int i=0;i<width*height;i++)
        {
            p[i]=color[j%9];
        }
    

        
}


int tp;

int click()
{
    int x1,y1;
    int x2,y2;

    bool xldone = false;
    bool yldone = false;
    static int j=0;

    struct input_event buf={0};
    while(1)
    {
        read(tp,&buf,sizeof(buf));
        if(buf.type==EV_ABS && buf.code==ABS_X&&!xldone)
        {
            x1=x2=buf.value;
            xldone=true;    
        }
        if(buf.type==EV_ABS && buf.code==ABS_Y&&!yldone)
        {
            y1=y2=buf.value;
            yldone=true;    
        }


        if(buf.type==EV_ABS && buf.code==ABS_X&&xldone)
            x2=buf.value;
        if(buf.type==EV_ABS && buf.code==ABS_Y&&yldone)
            y2=buf.value;
        if(buf.type==EV_KEY && buf.code==BTN_TOUCH && buf.value==0)
        {
            
            if(abs(x1-x2)<20&&abs(y1-y2)<20)
            {
                
                j++;
                break;
            }
            else
            {
                xldone=false;
                yldone=false;
            }
        }


    }
    return j;
    
}

int main(int argc, char *argv[])
{
    tp=open("/dev/input/event6",O_RDWR);
    if(tp<0)
    {
        perror("open");
        return -1;
    }
    while(1)
    {
        int num=click();
        color_lcd(&color[0],num);
    }

    return 0;



}