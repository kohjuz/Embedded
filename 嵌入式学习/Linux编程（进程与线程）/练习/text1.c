#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void)
{
    printf("[%d]fork之前\n",__LINE__);
    pid_t pid=fork();

    //parent
    if(pid>0)
    { 

        execl("./parent1","parent1",NULL);
    }
    //child
    else if(pid==0)
    {   
        execl("./child1","child1",NULL);
    }


    return 0;
}