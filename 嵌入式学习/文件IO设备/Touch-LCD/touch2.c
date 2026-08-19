#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input.h>

int main(int argc, char *argv[])
{
    int fd = open("/dev/input/event6", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }

    struct input_event buf;
    int last_x = 0;   // 缓存 x
    int last_y = 0;   // 缓存 y
    int touch  = 0;   // 缓存触摸状态（0=抬起 1=按下）

    while (1)
    {
        read(fd, &buf, sizeof(buf));

        // 只更新缓存，不打印
        if (buf.type == EV_KEY && buf.code == BTN_TOUCH)
        {
            touch = buf.value;
        }

        if (buf.type == EV_ABS && buf.code == ABS_X)
        {
            last_x = buf.value;
        }

        if (buf.type == EV_ABS && buf.code == ABS_Y)
        {
            last_y = buf.value;
        }

        // 一帧结束：X、Y、触摸状态都已是最新，在这里打印
        if (buf.type == EV_SYN)
        {
            printf("touch=%s (%d, %d)\n", touch ? "on" : "off", last_x, last_y);
        }
    }

    close(fd);
    return 0;
}
