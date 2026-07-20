#include<stdio.h>
int main()
{
    int a[4]={0,1,2,3};
    int b[4]={9,8,7,6};
    int c[4]={};
    for(int i=0;i<4;i++)
    {
        c[i]=a[i]+b[i];
        printf("%d\n",c[i]);
    }






    return 0;

}