#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/sysmacros.h>   // major() / minor()

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("%s <目录>\n", argv[0]);
        return -1;
    }

    struct stat dbuf;
    if (stat(argv[1], &dbuf) == -1) { perror("stat"); return -1; }
    if (!S_ISDIR(dbuf.st_mode)) {
        printf("不是目录文件\n");
        return -1;
    }

    DIR *dp = opendir(argv[1]);
    if (dp == NULL) { perror("opendir error"); return -1; }

    struct dirent *ep;
    while ((ep = readdir(dp)) != NULL) {
        if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
            continue;

        // 关键修复：拼出 "目录/文件名"，对每个文件单独 stat
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", argv[1], ep->d_name);

        struct stat buf;
        if (stat(path, &buf) == -1) { perror("stat"); continue; }

        // 类型字符：用 st_mode 判定（比 d_type 更稳，d_type 可能为 DT_UNKNOWN）
        char type;
        if      (S_ISREG(buf.st_mode))  type = '-';
        else if (S_ISDIR(buf.st_mode))  type = 'd';
        else if (S_ISCHR(buf.st_mode))  type = 'c';
        else if (S_ISBLK(buf.st_mode))  type = 'b';
        else if (S_ISFIFO(buf.st_mode)) type = 'p';
        else if (S_ISLNK(buf.st_mode))  type = 'l';
        else if (S_ISSOCK(buf.st_mode)) type = 's';
        else                            type = '?';
        printf("%c", type);

        printf(" %o",   buf.st_mode & 0777);   // 权限(后9位)
        printf(" %ld",  buf.st_nlink);          // 链接数
        printf(" %s",   getpwuid(buf.st_uid)->pw_name);   // 用户
        printf(" %s",   getgrgid(buf.st_gid)->gr_name);   // 组

        // 设备号 or 大小：只有块/字符设备才有设备号
        if (type == 'b' || type == 'c')
            printf(" %u,%u", major(buf.st_rdev), minor(buf.st_rdev));
        else
            printf(" %ld", buf.st_size);

        // 时间：st_mtime=内容修改时间；用 ctime 转成可读字符串
        char *ts = ctime(&buf.st_mtime);
        ts[strlen(ts) - 1] = '\0';
        printf(" %s", ts);

        printf(" %s\n", ep->d_name);            // 文件名
    }
    closedir(dp);
    return 0;
}
