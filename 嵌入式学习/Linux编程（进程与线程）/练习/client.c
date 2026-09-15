#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ipc.h>

int main(void)
{
    mkfifo("/tmp/fifo",0666);
    int fifofd=open("/tmp/fifo",O_RDONLY);
    int logfd=open("/tmp/log.txt",O_WRONLY|O_CREAT|O_APPEND,0777);
    char buf[1024]={0};
    while(1)
    {
        bzero(buf,sizeof(buf));
    ssize_t n = read(fifofd, buf, sizeof(buf));
    if(n <=0 ) break;
    write(logfd, buf, n); 
    }


    return 0;


}