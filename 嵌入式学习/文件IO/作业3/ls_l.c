#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>




int main(int argc, char *argv[])
{

    if(argc!=2)
    {
        printf("%s <file>",argv[0]);
        return -1;
    }
    struct stat buf;
    bzero(&buf,sizeof(buf));   
    stat(argv[1],&buf);
    if(!S_ISDIR(buf.st_mode))
    {
        printf("不是目录文件\n");
        return -1;
    }
    
    
    DIR *dp = opendir(argv[1]);
    if(dp == NULL)
    {
        perror("opendir error");
        return -1;
    }
    
    struct dirent *ep;
    char fullpath[1024];
    while((ep=readdir(dp))!=NULL)
    {
        
        if(strcmp(ep->d_name,".")==0 || strcmp(ep->d_name,"..")==0)
        {
            continue;
        }
         snprintf(fullpath, sizeof(fullpath), "%s/%s", argv[1], ep->d_name);

    // 获取该文件的属性
    if (stat(fullpath, &buf) == -1) 
    {
        perror("stat error");
        continue;
    }
        //首先判断文件是普通文件还是特殊文件
        //然后输出权限开头的单个字符
        if(S_ISREG(buf.st_mode))printf("r");
        else if(S_ISDIR(buf.st_mode))printf("d");
        else if(S_ISCHR(buf.st_mode))printf("c");
        else if(S_ISBLK(buf.st_mode))printf("b");
        else if(S_ISFIFO(buf.st_mode))printf("p");
        else if(S_ISLNK(buf.st_mode))printf("l");
        else if(S_ISSOCK(buf.st_mode))printf("S");
        else  printf("?");
        //+st_mode文件后9位文件的权限
        int n=0400;
        char perm[]={'r','w','-'};
        for(int i=0;i<9;i++)
        {
            printf("%c",(n>>i)&buf.st_mode? perm[i%3]: '-');
        }
        
        //+链接数
        printf(" %ld ",buf.st_nlink);
        // +用户
        printf(" %s ",getpwuid(buf.st_uid)->pw_name);
        // +组
        printf(" %s ",getgrgid(buf.st_gid)->gr_name);
        // +文件大小
        printf(" %ld ",buf.st_size);
        // +修改时间
        
        char *t=ctime(&buf.st_ctime);
        t[strlen(t)-1]='\0';
        printf(" %s ",t);
        printf("\033[1;5;32;40m%s\033[m ",ep->d_name);
        printf("\n");
    }
    closedir(dp);




    return 0;
}
/*  
普通文件没有设备号，只有存储在某个地方那个地方的设备号，有的设备号又是8，1  8，2，
我怎么区分我目录文件的普通文件和特殊文件，特殊文件还有7总类型我要去switch，
然后还要读取他们的权限是st_mode后面9位，然后链接数和用户又要读取，还要读取组，
修改时间可以用st_ctime 和st_mtime和st_atime读取，文件名我已经读取了

*/