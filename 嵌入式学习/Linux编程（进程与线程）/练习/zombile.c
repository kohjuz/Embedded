#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

void cleanup(int sig)
{
    static int cnt,count;
    while(waitpid(-1,NULL,WNOHANG)>0)count++;printf("clean:%d\n",count);
    cnt++;
    printf("进入这个清理函数%d次\n",cnt);


}

int main()
{
    signal(SIGCHLD,cleanup);
    for(int i=0;i<20;i++)
    {
    if(fork()==0)
    return 0;
    }

    while(1)
    {   
        pause();
    }
    return 0;





    
}