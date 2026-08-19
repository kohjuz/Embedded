/*
 * 课堂练习：chdir() 与 getcwd()
 *
 * 功能：在程序内使用 chdir() 切换工作目录，
 *       并用 getcwd() 获取/输出切换前后的路径，观察变化。
 *
 * 编译运行：
 *   gcc chdir_getcwd.c -o chdir_getcwd
 *   ./chdir_getcwd
 *
 * 思考：
 *   1. chdir() 改变的是"当前进程"的工作目录，而不是磁盘上的目录结构；
 *   2. 程序退出后，终端所在目录不受影响；
 *   3. getcwd() 返回的是逻辑路径（可能含符号链接），
 *      需要物理路径时可使用 realpath()。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 256

static void print_cwd(const char *tag)
{
    char buf[BUF_SIZE];

    if (getcwd(buf, sizeof(buf)) == NULL) {
        fprintf(stderr, "[%s] getcwd 失败: %s\n", tag, strerror(errno));
        return;
    }

    printf("[%s] 当前工作目录: %s\n", tag, buf);
}

int main(void)
{
   
    print_cwd("切换前");


    if (chdir("..") != 0) {
        fprintf(stderr, "chdir(\"..\") 失败: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    print_cwd("切换到 .. 后");


    if (chdir("/tmp") != 0) {
        fprintf(stderr, "chdir(\"/tmp\") 失败: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    print_cwd("切换到 /tmp 后");


    return 0;
}
