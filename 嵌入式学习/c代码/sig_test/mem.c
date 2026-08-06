#include<stdio.h>

int main(int argc,char const *(argv[]))
{
    if(argc!=3)
    {
        printf("请输入./文件名 参数 参数\n");

        return -1;
    }
    

    printf("所有外部参数：\n");
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    
printf("需要输出的bcd: %s\n", argv[1]+1);
printf("需要输出的字符：%c\n", argv[2][2]);

    return 0;

}