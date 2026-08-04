#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <dirent.h>


#define BUF_SIZE 1024

static int copy_recursive(const char *src, const char *dst) 
{
    struct stat st;
    if (stat(src, &st) == -1) {
        perror("stat src");
        return -1;
    }



    // 如果是目录
    if (S_ISDIR(st.st_mode)) {
        // 1. 检查目标目录是否存在
        struct stat dst_st;
        if (stat(dst, &dst_st) == -1) {
            // 目标不存在，创建
            if (mkdir(dst, 0755) == -1) {
                perror("mkdir");
                return -1;
            }
        } else if (!S_ISDIR(dst_st.st_mode)) {
            // 目标存在但不是目录，报错
            fprintf(stderr, "目标已存在但不是目录: %s\n", dst);
            return -1;
        }

        // 2. 打开源目录
        DIR *dp = opendir(src);
        if (!dp) {
            perror("opendir");
            return -1;
        }

        struct dirent *ep;
        char sub_src[1024], sub_dst[1024];
        while ((ep = readdir(dp)) != NULL) {
            if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
                continue;

            snprintf(sub_src, sizeof(sub_src), "%s/%s", src, ep->d_name);
            snprintf(sub_dst, sizeof(sub_dst), "%s/%s", dst, ep->d_name);

            if (copy_recursive(sub_src, sub_dst) == -1) {
                closedir(dp);
                return -1;
            }
        }
        closedir(dp);
        return 0;
    }

    // 其他类型（链接、设备等）暂不处理
    fprintf(stderr, "忽略特殊文件: %s\n", src);
    return -1;
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("%s <源文件> <目标文件>\n", argv[0]);
        return -1;
    }
    //首先判断是源文件是目录还是文件，如果是目录，就递归复制目录
    //如果是文件，就直接复制文件
    struct stat st;
    if (stat(argv[1], &st) == -1)
    {
        perror("stat error");
        return -1;
    }
    // 递归复制目录



    if (S_ISDIR(st.st_mode))
    {
        printf("是目录文件\n");
        //如果是目录，就递归复制目录
        //判断目标目录是否存在
        
        //如果目标目录不存在，就创建目标目录

        //递归复制目录
        copy_recursive(argv[1], argv[2]);
        printf("复制目录目录成功\n");
        return 0;

    }






    //如果是普通文件，就直接复制文件
    printf("是普通文件\n");

    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
        perror("fopen source error");
        return -1;
    }

    FILE *fp2 = fopen(argv[2], "wb");
    if (fp2 == NULL)
    {
        perror("fopen dest error");
        fclose(fp);
        return -1;
    }

    char buf[BUF_SIZE];
    size_t n;
    while ((n = fread(buf, 1, BUF_SIZE, fp)) > 0)
    {
        if (fwrite(buf, 1, n, fp2) != n)
        {
            perror("fwrite error");
            fclose(fp);
            fclose(fp2);
            return -1;
        }
    }

    if (ferror(fp))
    {
        perror("读取文件失败");
        fclose(fp);
        fclose(fp2);
        return -1;
    }

    fclose(fp);
    fclose(fp2);
    printf("复制完成\n");
    return 0;
}
