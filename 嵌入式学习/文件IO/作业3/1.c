#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("%s <flie>",argv[0]);
        return -1;
    }
    
    struct stat st;
    stat(argv[1],&st);
    printf("文件大小为%ld\n",st.st_size);

    return 0;
}



