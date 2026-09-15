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

#include "daemon.h"

// void daemon_init(void);

// int main(void)
// {
//     // daemon_init();
    




//     return 0;
// }
void daemon_init(void);
int main(int argc, char **argv)
{
    daemon_init();
	mkfifo("/tmp/fifo", 0777);
	int fd = open("/tmp/fifo", O_WRONLY);

	char buf[1024];
	time_t t;
	while(1)
	{
		bzero(buf, 1024);

		time(&t);
		snprintf(buf, 1024, "[%-6d] %s",getpid(), ctime(&t));

		write(fd, buf, strlen(buf));
		sleep(1);
	}

	return 0;
}