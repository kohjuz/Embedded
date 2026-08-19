#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include <stdlib.h>

void main(int argc,char **argv)
{
    // system("ls -l");
    int fd = open("a.txt",O_RDWR|O_CREAT|O_TRUNC);
    if(fd==-1)
    {
        perror("error");
    }
    dup2(fd,1);
    system("ls -l");
    close(fd);
}