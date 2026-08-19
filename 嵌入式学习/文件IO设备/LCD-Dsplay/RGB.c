#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define ARGB(a,r,g,b) ((a)<<24|(r)<<16|(g)<<8|(b))

int main(int argc, char *argv[])
{
    int fd =open("/dev/fb0",O_RDWR);
    if(fd==-1)
    {
        perror("open ");
        return -1;
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
        return -1;
    }
    //???¡¤?????¨¬????
    int color[9]=
    {
        ARGB(0,0,0,0),//???¡Â
        ARGB(0,255,0,0),
        ARGB(0,0,255,0),//??
        ARGB(0,255,255,0),//????
        ARGB(0,0,0,255),//????
        ARGB(0,255,0,255),//?¡¤?¨¬??
        ARGB(0,0,255,255),//?¨¤??
        ARGB(0,255,255,255),//¡ã¡Á??
        ARGB(255,0,0,0) //????
    };

    while (1)
    {
        //?????¨ª?????¨²?¨¬?¡§?¨¢??: ¡ã¡Á/??/?¨¬??
        for(int i=0;i<(width/3)*height;i++)
        {
            p[i]=color[7];
        }
        
        for(int i=(width/3)*height;i<(width*2/3)*height;i++)
        {
            p[i]=color[4];
        }
        
        for(int i=(width*2/3)*height;i<width*height;i++)
        {
            p[i]=color[1];
        }
        usleep(1000000);

        // ???¨²?¨²?¨¬?¡§?¨¢??: ??/?¨¬/????
        for(int i=0;i<(width/3)*height;i++)
        {
            p[i]=color[8];
        }
        
        for(int i=(width/3)*height;i<(width*2/3)*height;i++)
        {
            p[i]=color[1];
        }
        
        for(int i=(width*2/3)*height;i<width*height;i++)
        {
            p[i]=color[3];
        }
        usleep(1000000);

        //¡¤¡§?¨²?¨²?¨¬?¡§?¨²??: ??/¡ã¡Á/?¨¬????¡Á¨®??????
        for(int y=0;y<height;y++)
        {
            for(int x=0;x<width/3;x++)          /* ¡Á¨® 1/3: ?? */
            {
                p[y*width+x]=color[4];
            }
            
            for(int x=width/3;x<width*2/3;x++)  /* ?? 1/3: ¡ã¡Á */
            {
                p[y*width+x]=color[7];
            }
            
            for(int x=width*2/3;x<width;x++)    /* ?? 1/3: ?¨¬ */
            {
                p[y*width+x]=color[1];
            }
        }
        usleep(1000000);
    }

    munmap(p,size);
    close(fd);

    return 0;
}
