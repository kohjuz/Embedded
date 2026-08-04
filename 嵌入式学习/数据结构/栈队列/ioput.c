#include <stdio.h>
#define MAX 100
int stake[MAX];
int top = -1;

void pupp(int n)
{
    if(top<MAX-1)
    {    
        top++;
        stake[top]=n;

    }
}
int pop()
{
    if(top>=0)
    {
        int n = stake[top];
        top--;
        return n;
    }
    return -1;
}
int isEmpty()
{
    return top==-1;
}
char hex_pop(int n)
{
    if(n<10)
    {
        return n+'0';
    }
    else
    {
        return n-10+'A';
    }
    
    
}
int n;
int main(int argc, char *argv[])
{
    printf("请输入一个十进制数:");
    int ret = scanf("%d", &n);
    if(ret!=1)
    {
        printf("输入错误,请输入一个十进制数\n");
               return -1;
    }
    if(n==0)
    {
        printf("十六进制数是：0\n");
        return 0;
    }
    int st=0;
    if(n<0)
    {
        st=-1;
        n=-n;
    }
    while(n>0)
    {
        pupp(n%16);
        n/=16;
    }
    if(st==-1)
    {
        printf("-");
    }
    printf("十六进制数是：0x");
    while(!isEmpty())
    {
        printf("%c",hex_pop(pop()));

    }
    return 0;
}