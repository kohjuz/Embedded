#include<stdio.h>
int str(int *arr,int n);
int main()
{
    int a[7] = {-2, 5, -1, 6, -4, -8, 6};
    int rd=str(a,7);
    printf("%d",rd);
 


    return 0;
}
int str(int *arr,int n)
{
    int *p=arr;
    int zstr=*p;
    int dstr=*p;
    *p++;
    for(;p<(arr+n);p++)
    {
        if(*p>dstr+*p)
        {
            dstr=*p;
        }
        else
        {
            dstr=*p+dstr;
        }
        if(dstr>zstr)
        zstr=dstr;

    }
    return zstr;


}