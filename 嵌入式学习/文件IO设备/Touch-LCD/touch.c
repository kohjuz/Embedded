#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input.h>


int main(int argc, char *argv[])
{
    int fd =open("/dev/input/event6",O_RDWR);
    if(fd<0)
    {
        perror("open");
        return -1;
    }

    struct input_event buf;
    while(1)
    {
        read(fd,&buf,sizeof(buf));
        

        
        // if(buf.type==EV_SYN)
        // {
        //     printf("-------- SYN --------\n");
        // }

        if(buf.type == EV_KEY && buf.code == BTN_TOUCH)
        {
            printf("%s\n", buf.value==0? "off":"on");
        }

        if(buf.type == EV_ABS && buf.code == ABS_X)
        {
            printf("x: %d\n", buf.value);
        }
        if(buf.type == EV_ABS && buf.code == ABS_Y)
        {

            printf("y: %d\n", buf.value);
        }
    }

    return 0;
}