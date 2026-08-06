#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>



int main(void)
{
    FILE *fp=fopen("a.txt","w");
    if(fp==NULL)
    {
        perror("打开文件失败");
        return -1;
    }

    
        struct stat st;
        //属性结构体
        /*
        struct stat
        {
            dev_t st_dev;//普通文件所在存储器设备号
            mode_t st_mode; //文件权限+文件类型
            ino_t st_ino;//i节点，相当于文件的唯一标识符
            nlink_t st_nlink;//文件的别名的数量
            uid_t st_uid;//文件所有者的用户ID
            gid_t st_gid;//文件所有者的组ID
            off_t st_size;//文件大小，单位字节
            blkcnt_t st_blocks;//文件占用的块数量
            dev_t st_rdev;//本文件设备号，特殊文件设备号

            time_t st_atime;//文件最近一次被访问的时间
            time_t st_mtime;//文件最近一次被修改的时间
            time_t st_ctime;//文件最近一次被修改的时间
            blksize_t st_blksize;//文件块大小，单位字节
            struct timespec
            {
                long    tv_sec;   //秒
                long    tv_nsec;  //纳秒
            };

            st_mode：16位 short 短整型数据

            高4位文件类型        | | | | |
            7种文件类型: r d c b f l s
            S_ISREG(st_mode)  is it a regular file?
            S_ISDIR(st_mode)  directory?
            S_ISCHR(st_mode)  character device?
            S_ISBLK(st_mode)  block device?
            S_ISFIFO(st_mode) FIFO (named pipe)?
            S_ISLNK(st_mode)  symbolic link?  (Not in POSIX.1-1996.)
            S_ISSOCK(st_mode) socket?  (Not in POSIX.1-1996.)


            中间三位分别是 setuid、setgid(只对普通文件有效) 和 stickyBit(只针对目录有效)
            @c://第一位 sticky bit：使得用户只能增加和删除属于自身的文件，不能删除别的用户的文件。
            @b://第二位sgid：使得文件的使用者获得文件所在目录的所属组的临时授权。
            @a://第三位suid：使得文件的使用者获得文件所有者的临时授权。

                |@a|@b|@c|
                
            后9位表达文件的权限，与三组权限一一对应。
            每组权限有3位，分别是读、写、执行权限。
            读：100
            写：010
            执行：001
            
                    | | | |    | | | |      | | | |

            

            

            
        }
        */
        long last_size=0;

    for(int i=0;i<10000;i++)
    {
        fputc('a',(fp));


        stat("a.txt",&st);
        if(st.st_size>last_size)
        {
            printf("写入%d字节后文件实际大小为%ld\n",i+1,st.st_size);
            printf("标准IO缓存区大小为%ld\n",st.st_size-last_size);
            break;
        }




    }


    fclose(fp);



    return 0;
}
