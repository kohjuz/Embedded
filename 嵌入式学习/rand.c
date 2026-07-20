#include<stdio.h>
#include<sys/types.h>
#include<sys/fcntl.h>

time_t t=time(NULL);
srand(t);

int main(int agc,char**agv)
{

    int n=rand();

    int fd = open(*agv,O_RDONLY);
    if(fd==-1)
    {
        perror("error");

    }
    else
    {
        read(fd,)

    }


    return 0;
}