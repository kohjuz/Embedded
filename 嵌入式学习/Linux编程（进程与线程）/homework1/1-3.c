#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>


int main(int argc, char *argv[])
{
    //产生3个子进程
    for (int i = 0; i < 3; i++)
    {
        pid_t pid = fork();

        if (pid == 0)//子进程
        {
            printf("I am child %d, my pid is %d,my parent pid is %d\n", i, getpid(),getppid());
            return i;
        }
        if(pid>0)//父进程
        {
           
            int status;
            wait(&status);
            if(WEXITSTATUS(status)==2)
            {
                 printf("I am parent my pid is %d\n",  getpid());
            }
        }
    }
    

    return 0;
}