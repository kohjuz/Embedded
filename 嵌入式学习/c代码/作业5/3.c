// #include<stdio.h>

// double myPower(double x,int N)
// {
//     if(N==0)
//         return 1.0;
//     if(N>0)
//         return x*myPower(x,N-1);

//     return 1.0/myPower(x,-N);


    
// }
// int main()
// {

//     printf("%.2lf",myPower(2,-1));
//     return 0;
// }
//上面是递归，下面是循环
#include<stdio.h>
double myPower(double x,int N)
{
    double res=1;
    if(N==0)
    return 1;
    
    if(N>0)
    {
        for(int i=1;i<=N;i++)
        {
            res*=x;
        }


    }
    else if(N<0)
    {
        for(int j=1;j<=-N;j++)
        {
           
            res*=x;
            
        }
        res=1/res;
    }
   return res;
}
int main()
{
    printf("%.2lf",myPower(2,-3));

    return 0;
}