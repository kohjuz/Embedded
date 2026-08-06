#include <stdio.h>
#include <stdint.h>

int main(void)
{
    int64_t a[4] = {1, 2, 3, 4};
    int64_t *p1=(int64_t *)(&a+1);
    int64_t *p2=(int64_t *)((int64_t)a+1);
 
    printf("%llx\n", p1[-1]);
    printf("%llx\n", *p2);
    

     return 0;
}