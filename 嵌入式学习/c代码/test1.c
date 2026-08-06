#include<stdio.h>
int n;
int i;
int fact_int=1;
long long fact_long=1;
int sum_int=0;
long long sum_long=0;
int main()
{
    printf("请输入一个正整数:");
    scanf("%d",&n);
    if(n<=20)
    {
        
                 for(i=1;i<=n;i++)
            {
                fact_int*=i;
                fact_long*=i;

                sum_int+=fact_int;
                sum_long+=fact_long;
            }
            
            printf("int:%d,long:%lld",sum_int,sum_long);
        
        

            

       

    }




    return 0;
}