#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h> 

int main(int argc,char **argv)
{
    int fd[2];
    pipe(fd);
    //父进程
    if(fork()>0)
    {
        dup2(fd[1],STDOUT_FILENO);
        execl("/bin/ls","ls","/etc",NULL);
        
    }
    //子进程
    else if(fork()==0)
    {
        dup2(fd[0],STDIN_FILENO);
        close(fd[1]);
        execl("/usr/bin/wc","wc","-w",NULL);
        
        

    }
    return 0;

}