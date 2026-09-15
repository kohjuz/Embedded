#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void fun(int sig)
{
    printf("收到信号%d\n",sig);
}

int main(void)
{
    signal(SIGINT,fun);
    if(mkfifo("/tmp/sigfifo",0666)==-1)
    {
        perror("mkfifo failed");
    }
    int sigfd=open("/tmp/sigfifo",O_WRONLY);
    if(sigfd==-1)
    {
        perror("open failed");
        exit(1);
    }
    printf("PID为：%d\n",getpid());
    pid_t pid=getpid();
    char buf[1024]={0};
    snprintf(buf, sizeof(buf), "%d", pid);
    write(sigfd,buf,strlen(buf));
    close(sigfd);

    while(1)
    {
        pause();
    }
    return 0;
}
