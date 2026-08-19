#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input.h>    
#include <stdbool.h>

#define ARGB(a,r,g,b) ((a)<<24|(r)<<16|(g)<<8|(b))

int tp;

void click()
{
    int x1,y1;
    int x2,y2;

    bool xldone = false;
    bool yldone = false;

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
                break;
            }
            else
            {
                xldone=false;
                yldone=false;
            }
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
        printf("单击\n");
    }

    return 0;



}