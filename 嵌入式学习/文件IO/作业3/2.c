#include <stdio.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>


int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        printf("%s <file>",argv[0]);
        return -1;
    }



    struct stat st;
    stat(argv[1],&st);




    return 0;
}