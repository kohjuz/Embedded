#include<stdio.h>

int max(int a,int b,int c)
{
    int max;
    if(a>b)
    {
        max=a;
    }
    else
    {
        max=b;
    }
    max>c ? max : c;
    return c;

}
int main()
{
    
    printf("max:%d",max(3,5,10));
    return 0;
}