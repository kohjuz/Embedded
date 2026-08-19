#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

void main (int argc, char *argv[])
{

    if(argc!=3)
    {
        printf("输入error 输入格式： <./文件> <被复制文件> <复制文件>");
        return ;
    }
    int fd =open(argv[1],O_RDONLY);
    if(fd==-1)
    {
        printf("%s open error",argv[1]);
        return;
    }
    int fd1 =open(argv[2],O_WRONLY|O_CREAT);
    if(fd1==-1)
    {
        printf("%s open error",argv[1]);
        return;
    }
    char buf[100];
    char *p;
    p=buf;


    while(1)
    {
        int read_number =read(fd,buf,sizeof(buf));
        if(read_number==-1)
        {
            perror("read() error");
            return;
        }
        if(read_number==0)
        {
            break;
        }
        while(read_number>0)
        {
                    
            int write_number = write(fd1,p,read_number);
            if(write_number==-1)
            {
                perror("write() error");
                return;
            }
            read_number-=write_number;
            p=p+write_number;

        }





    }
    close(fd);
    close(fd1);


}
