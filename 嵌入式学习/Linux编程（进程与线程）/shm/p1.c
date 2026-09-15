#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>


#define SHM_SIZE 1024

int main(void)
{
    
    key_t key=ftok("./", 1);
    int shmid=shmget(key, 1024, IPC_CREAT|0666);
    if(shmid<0)
    {
        perror("shm failed");
        exit(EXIT_FAILURE);
    }
    char *shm=shmat(shmid, NULL, 0);
    if(shm==(void*)-1)
    {
        perror("shm failed");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_w = sem_open("/sem_w",0);
    sem_t *sem_r = sem_open("/sem_r",0);
    if(sem_w == SEM_FAILED || sem_r == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    char buf[256];
    while(1)
    {
        sem_wait(sem_r);
        printf("P1收到%s",shm);
        if(strcmp(shm,"quit\n")==0)
        {
            
            sem_post(sem_w);
            break;

        }
        sem_post(sem_w);




    }
    sem_close(sem_w);
    sem_close(sem_r);
    unlink("/sem_w");
    unlink("/sem_r");
    shmdt(shm);
    return 0;

}