#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/un.h>

int main(void)
{
    if(mkfifo("/tmp/sigfifo",0666)==-1)
    {
        perror("mkfifo failed");
       
    }
    int sigfd=open("/tmp/sigfifo",O_RDONLY);
    if(sigfd==-1)
    {
        perror("open failed");
        
    }
    char buf[1024]={0};
    
        
        read(sigfd,buf,sizeof(buf));

        close(sigfd);
        int pid=atoi(buf);
    while(1)
    {   
        kill(pid,SIGINT);
        getchar();
    }
    
    return 0;
}