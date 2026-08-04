#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[])
{

    if(argc!=2)
    {
        printf("输入格式error : (./%s  +   文件名)\n",argv[0],strerror(errno));
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
    int n=0;
    printf("请输入想要抽的人数：\n");
    int ret=scanf("%d",&n);
    if(ret!=1)
    {
        printf("输入格式错误：\n");
        while(getchar()=='\n');
        return 1;
    }
    
    char buf[150];
    int buf_number=0;
    char name[150][50];
    while(fgets(buf,sizeof(buf),fp)!=NULL && buf_number<150)
    {   
        buf[strcspn(buf,"\n")]='\0';
        strcpy(name[buf_number],buf);
        ++buf_number;
    }
    // printf("\r第%d行%s",buf_number,buf);
    for(int i=0;i<n;i++)
    {    time_t t= time(NULL);
    srand(t);
    int a=rand()%buf_number;
    printf("正在抽奖\n");

    int total_rounds = 25 + rand() % 10;
    int final_round = total_rounds - 3;

    for(int i=0;i<total_rounds;i++)
    {
        int b=rand()%buf_number;
        printf("\r%-20s",name[b]);
        fflush(stdout);

        if(i < final_round)
        {
            double progress = (double)i / final_round;
            int delay = 10000 + (int)(progress * progress * 90000);
            usleep(delay);
        }
        else
        {
            int delay = 100000 + (i - final_round) * 80000;
            usleep(delay);
        }
    }

    printf("\r%s",name[a]);
    fflush(stdout);
    printf("\n");
        
    }



 



    fclose(fp);

    return 0;
}