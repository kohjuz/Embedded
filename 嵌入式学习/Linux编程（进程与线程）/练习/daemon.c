#include "daemon.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   // 新增，open /dev/null 需要

void daemon_init(void)
{
    pid_t a;
    int max_fd, i;

    signal(SIGHUP, SIG_IGN);

    a = fork();
    if (a > 0)
        exit(0);

    setsid();

    a = fork();
    if (a > 0)
        exit(0);

    setpgid(0, 0);

    max_fd = sysconf(_SC_OPEN_MAX);
    for (i = 0; i < max_fd; i++)
    {
        close(i);
    }

    // ==========关键新增这三行！占住fd 0,1,2，防止业务open抢编号==========
    open("/dev/null", O_RDWR);   // fd 0
    open("/dev/null", O_RDWR);   // fd 1
    open("/dev/null", O_RDWR);   // fd 2

    umask(0);
    chdir("/");
}
