#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

void main(int arc,char *arv[])
{
    if(arc!=3)
    {
        printf("错误，请输入：./文件名 被复制文件 复制文件");
        return;
    }
    int fd = open(arv[1],O_RDONLY);
    if(fd==-1)
    {
        perror("open error");
    }
        int fd1 = open(arv[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
    if(fd1==-1)
    {
        
        perror("open error");
    }
    char buf[100];
    char *p;
    p=buf;
    while(1)
    {
        int rd_number = read(fd,buf,100);
        if(rd_number==-1)
        {
            perror("read() error");
            return;
        }
        if(rd_number==0)
        return;

        while(rd_number>0)
        {
                int wr_number = write(fd1,p,rd_number);
                    if(wr_number==-1)
                    {
                        perror("write() error");
                        return;
                    }
            rd_number-=wr_number;
            p+=wr_number;

        }
 
    }
    
       close(fd);
        close(fd1);

   
}