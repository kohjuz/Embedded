#include<stdio.h>
int a,b;
int sum(int n);
void main()
{

    printf("请输入一个数字n:\n");
    scanf("%d",&a);
    b=sum(a);
    printf("n+n-1+······1=:%d",b);


}

int sum(int n)
{
    if(n==1)
    return 1;

    return sum(n-1)+n;
}