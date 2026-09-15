#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>


void main(void)
{
    for(int i=0;i<3;i++)
    {
        printf("还有%d秒退出\n",3-i);
        sleep(1);
    }
        //printf("[%d]: ppid %d pid %d\n",__LINE__,getppid(),getpid());
    exit(1);
}