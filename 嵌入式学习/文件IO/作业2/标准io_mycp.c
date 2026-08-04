#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>



int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        printf("输入格式error : (./%s  +   %s文件名)\n",argv[0],strerror(errno));
    }
    else
    {
        printf("输入格式正确\n");
    }
    FILE* fp=fopen(argv[1],"r+");
    if(fp==NULL)
    {
        perror("打开文件失败");
        return 1;
    }
    FILE* fp2=fopen(argv[2],"w+");
    if(fp2==NULL)
    {
        perror("打开文件失败");
        return 1;
    }
    // int ret=0;
    //每字符读取，fgetc();getc();
    //每字符写， fput();
    // while(1)
    // {
    //     ret=fgetc(fp);
    //     if(ret==EOF)
    //     {
    //         if(feof(fp))
    //         {
    //             break;
    //         }
    //         perror("读取文件失败");
    //         break;
    //     }
    //     fputc(ret,fp2);
    // }

    //每行读取, fgets(),gets();
    //每行写 ，fputs();

    /*
    用读取每一行，写入每一行
    char *buf=malloc(1024);
    while(1)
    {   
        
        if(fgets(buf,1024,fp)==NULL)
        {
            if(feof(fp))
            {
                break;
            }
            if(ferror(fp))
            {
                perror("读取文件失败");
                break;
            }
        }
        
        fputs(buf,fp2);

        
    }

    printf("复制完成\n");

*/

// 用fread()和fwrite()复制文件

char buf[20*5];
int nread,begin,end;

while(1)
{
    begin=ftell(fp);
    nread=fread(buf,20,5,fp);
    if(nread<5)
    {

        if(feof(fp))
        {
            end=ftell(fp);
            fwrite(buf,end-begin,1,fp2);
            break;
        }
        if(ferror(fp))
        {
            perror("读取文件失败");
            break;
        }
    }
    fwrite(buf,20,nread,fp2);



}




    return 0;
}