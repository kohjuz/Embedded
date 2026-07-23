#include<stdio.h>



#define MIN(a,b) \
            ({ \
                typeof(a) _a=a; \
                typeof(a) _b=b; \
                _a > _b ? _b : _a; \
            })

int main(int argc,char **argv)
{
    int a=2,b=5;
    int m=MIN(a,b);
    printf("%d %d %d",a,b,m);

    return 0;
}