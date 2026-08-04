#include <stdio.h>

int main(void)
{
    // 定义一个4字节的整型数据
    int a = 0x12345678;

    // 定义一个char型指针指向最低地址
    char *p = &a;
    int *z;
    z = p;
    printf("%#x\n", *z);

    // 将最低字节数据打印出来
    // 如果是0x78，那就代表最低地址存储了低有效位，是小端序
    // 如果是0x12，那就代表最低地址存储了高有效位，是大端序
    printf("%#x\n", *p);
    
}