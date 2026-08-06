#include<stdio.h>

int Feb(int N)
{
    if(N<2)
    return N;

    return Feb(N-2)+Feb(N-1);
}

int main()
{
    int n;
    printf("ÇëÊäÈëÕûÊýN:");
    scanf("%d",&n);
    Feb(n);
    printf("%d",Feb(n));



    return 0;
}