#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

int main(void)
{
	if(mkfifo("/tmp/fifo",0666)==-1)
	{
		perror("mkfifo failed");
	}
	int fd = open("/tmp/fifo",O_WRONLY);
	if(fd == -1)
	{
		perror("open failed");
		exit(1);
	}
	char buf[1024] = {0};
	while(1)
	{
		printf("请输入要发送的消息: ");
		bzero(buf,sizeof(buf));
		fgets(buf,sizeof(buf),stdin);
		write(fd,buf,strlen(buf));
	}
	close(fd);
	return 0;


}
