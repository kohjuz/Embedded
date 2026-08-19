#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <stdint.h>
void main(int argc, char **argv)
{
    int fd[2];
    pipe(fd);

    long state=fcntl(fd[0],F_GETFL);
    state |=O_NONBLOCK;
    fcntl(fd[0],F_SETFL,state);

    char buf[3];
    if(read(fd[0],buf,3)>0)
    {
        printf("%s",buf);
    }
    else
    {
        printf("读不到\n");

    }




}