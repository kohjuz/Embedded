#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* 自报错的open封装：出错库内部直接打印错误 */
int my_open(const char *path,int flag)
{
    int fd = open(path,flag);
    if(fd < 0)
    {
        // ?自报错：库内部自己打印错误，调用者不用写perror
        perror("[my_open] open error");
    }
    return fd;
}

ssize_t my_read(int fd,void *buf,size_t count)
{
    ssize_t ret = read(fd,buf,count);
    if(ret < 0)
    {
        perror("[my_read] read error");
    }
    return ret;
}

ssize_t my_write(int fd,void *buf,size_t count)
{
    ssize_t ret = write(fd,buf,count);
    if(ret < 0)
    {
        perror("[my_write] write error");
    }
    return ret;
}

int my_close(int fd)
{
    int ret = close(fd);
    if(ret < 0)
    {
        perror("[my_close] close error");
    }
    return ret;
}
