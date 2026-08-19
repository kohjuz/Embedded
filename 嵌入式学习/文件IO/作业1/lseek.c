#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

void main(int arc,char **argv)
{
    int fd =open ("a.txt",O_RDWR);

    char buf[10240+100];
    lseek(fd,10240,SEEK_SET);
    char buf2[100];
    scanf("%s",buf2);
    write(fd,buf2,100);


    

}