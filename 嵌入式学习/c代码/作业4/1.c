//判断大小端
#include<stdio.h>
unsigned int a;
void pkt(int pkt)
{
    char *p;
    p=(char*)&pkt;
    if(*p= 0x78)
    {
        printf("小端");
    }
    else
    {
        printf("大端");
    }
}
void main()
{
    printf("请输入一个int型数据:");
    scanf("%d",&a);

    pkt(a);
    




}