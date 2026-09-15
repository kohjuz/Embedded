#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if(pid == 0)
    {
        char buf[20] = {0};
        read(fd[0],buf,20);
        printf("from parent: %s\n",buf);
        
        write(fd[1],"1234567890",10);

    }
    else if(pid > 0) 
    {
        sleep(2);
        char buf[20] = "hello world";
        write(fd[1],buf,20);
        usleep(1*1000);

        char buf2[20] = {0};
        read(fd[0],buf2,20);
        printf("from child: %s\n",buf2);

    }


    return 0;
}