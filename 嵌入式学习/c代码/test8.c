#include <stdio.h>

float count(int n)
{
    float s=0;

    for(int i=1;i<n;i++)
    {
        s=1.0/((float)sum(i));
    }

    return s;
}

int sum(int n)
{
    if(n==0)
    return 0;
    return sum(n-1)+n;

}

int main()
{
    printf("%d\n", sum(11));
    printf("%f\n", count(11));
    return 0;
}