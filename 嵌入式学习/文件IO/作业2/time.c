#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


int main(int argc, char *argv[])
{
    int count=0;
    FILE *lf = fopen("line", "rb");
    if (lf != NULL) 
    {
        fread(&count, sizeof(int), 1, lf);   
        fclose(lf);
    }
    // 文件不存在count保持0下面++后从1 开始


    while(1)
    {
        
        time_t now=time(NULL);
        char * t=ctime(&now);
        t[strlen(t)-1]='\0';//ctime()返回的字符串以'\n'结尾，需要去掉
        //读取上次打印的行号，从上次结束行号开始打印


        ++count;//比如说我打印了一次行号和时间，那么行号就是1，下一次就是2，以此类推
        printf("%d   %s\n",count,t);
        //把每次的行号和时间写入文件中time.txt中,下次执行文件会从上次结束行号开始打印
        FILE* fp=fopen("time.txt","a+");//追加写入
        if(fp==NULL)
        {
            perror("打开文件失败");
            return 1;
        }
        else
        {
            fprintf(fp,"%d   %s\n",count,t);
            fflush(fp);
            lf = fopen("line", "wb");
            fwrite(&count, sizeof(int), 1, lf);
            fclose(lf);

        }
        fclose(fp);
        
        fflush(fp);
        sleep(1);


    }
    


    return 0;
}
