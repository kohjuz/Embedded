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
#include <string.h>
#include <errno.h>
#include <stdint.h>



#include "jpeg_display.h"


void picture_display_on()
{
    
        struct imageInfo img;
        if(jpg2rgb("./on.jpg",&img)!=0)
        {
            perror("jpg2rgb");
            return;
        }
        FBInfo fb;
        if(fb_open(&fb)!=0)
        {
            free(img.rgb);
            perror("fb_open");
            return;
        }
        fb_clear(&fb);
        int scale=1000;
        fb_show_image(&fb,&img,scale);
        fb_close(&fb);
        free(img.rgb);



}
void picture_display_off()
{
     struct imageInfo img;
    if(jpg2rgb("./off.jpg",&img)!=0)
    {
        perror("jpg2rgb");
        return;
    }
    FBInfo fb;
    if(fb_open(&fb)!=0)
    {
        free(img.rgb);
        perror("fb_open");
        return;
    }
    fb_clear(&fb);
    int scale=1000;
    fb_show_image(&fb,&img,scale);
    fb_close(&fb);
    free(img.rgb);

}






int tp;

int click()
{
    int x1,y1;
    int x2,y2;
    bool xldone = false;
    bool yldone = false;
   

    struct input_event buf={0};
    while(1)
    {
        read(tp,&buf,sizeof(buf));
        // if(buf.type==EV_ABS && buf.code==ABS_X&&!xldone)
        // {
        //     x1=x2=buf.value;
        //     xldone=true;
        

        // }
        // if(buf.type==EV_ABS && buf.code==ABS_Y&&!yldone)
        // {
        //     y1=y2=buf.value;
        //     yldone=true;    
        // }


        // if(buf.type==EV_ABS && buf.code==ABS_X&&xldone)
        //     x2=buf.value;
        // if(buf.type==EV_ABS && buf.code==ABS_Y&&yldone)
        //     y2=buf.value;
        // if(buf.type==EV_KEY && buf.code==BTN_TOUCH && buf.value==0)
        // {
        //     if(abs(x1-x2)<20&&abs(y1-y2)<20)
        //     {
        //         break;
                
        //     }
        //     else
        //     {
        //         xldone=false;
        //         yldone=false;
        //     }
        // }
        if(buf.type==EV_KEY && buf.code==BTN_TOUCH && buf.value==1)
        {
            picture_display_on();
        }
        if(buf.type==EV_KEY && buf.code==BTN_TOUCH && buf.value==0)
        {
            picture_display_off();
        }
        


    }
    
    
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
        click();
        
    }

    return 0;



}