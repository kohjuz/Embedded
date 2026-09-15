#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>


void main(void)
{
        int status;
        // wait(&status);
        while(1)
        {
            if(waitpid(-1,&status,WNOHANG)<=0)
            {
                printf("先干点别的事情\n"); 
                sleep(1);
            }
            else 
            {
                printf("成功退出\n");
                break;
            }

        }
        printf("[%d]: ppid %d pid %d\n",__LINE__,getppid(),getpid());
}