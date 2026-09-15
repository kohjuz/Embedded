#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>


void *f(void *arg)
{
    for(int i=0; ; i++)
    {
        
        if(i<26)
        {
            fprintf(stderr,"%c",'a'+i);
            usleep(100*1000);
        }
        else
        {
            printf("\n");
            break;
        }

    }
    pthread_exit("abcd");
}
int main(int argc,char const*argv[])
{

    //初始化属性变量，并将分离属性加进去
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    //创建独立运行的线程
    pthread_t tid;
    pthread_create(&tid,&attr,f,NULL);


    void *ret;
    if((errno=pthread_join(tid, &ret)) != 0)
        perror("接合线程失败");    


    pthread_exit(NULL);
    
}