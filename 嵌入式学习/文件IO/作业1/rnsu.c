#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

//读取一个txt文件，并将所有包含数字相加后输出到屏幕上
void main(int argc,char *argv[])
{
    if(argc!=2)
    {
        printf("错误，请输入：./文件名 文件名(xxx.txt)");
        return;
    }
    int fd = open(argv[1],O_RDONLY);
    if(fd==-1)
    {
        perror("open error");
    }
    char buf[100];
    char *p;
    p=buf;
    int sum=0;
    int cur=0;
    while(1)
    {
        int rd_number = read(fd,buf,100);
        if(rd_number==-1)
        {
            perror("error");
            return;
        }
        if(rd_number==0)
        {
            break;
        }
        
 
        //判断是不是数字,是数字就加到sum中,然后打印到屏幕
        for(int i=0;i<=rd_number;i++)
        {
            //如果读到空格,换行，字符，就把cur清零
            if(buf[i]>='0'&&buf[i]<='9')
            {
                cur=cur*10+buf[i]-'0';

            }
            else
            {
                sum+=cur;
                // printf("%d\n",cur);
                cur=0;

            }
        }

 
        
    }
    
    printf("%d\n",sum);
    close(fd);

}
