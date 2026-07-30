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
    char *buf=malloc(1024);
    while(1)
    {   
        fgets(buf,1024,fp);
        if(strcmp(buf,"\n")==0)
        {

            if(ferror(fp))
            {
                perror("读取文件失败");
                break;
            }

            break;
        }

        fputs(buf,fp2);
        fputs("\n",fp2);
        memset(buf,0,1024);
        continue;

        
    }
    fclose(fp);
    fclose(fp2);
    free(buf);
    buf=NULL;
    fp2=NULL;
    fp=NULL;
    printf("复制完成\n");





    return 0;
}