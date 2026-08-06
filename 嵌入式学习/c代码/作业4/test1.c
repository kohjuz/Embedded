#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{

    for(int i=1; ;i++)
    {
        int val = i % 11;
        if(val == 0) val = 11; 
        val = val > 10 ? 1 : val; 
        printf("%d\t", val);
        if(val == 10)
            printf("\n");
        usleep(100*1000);
        if(i > 100) break; 
    }

    return 0;
}