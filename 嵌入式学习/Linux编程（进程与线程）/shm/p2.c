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
    int shmid=shmget(key, 1024, IPC_CREAT | IPC_EXCL |0666);
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
    //清除混乱数据
    memset(shm,0,SHM_SIZE);

    sem_t *sem_w = sem_open("/sem_w",O_CREAT,0666,1);
    sem_t *sem_r = sem_open("/sem_r",O_CREAT,0666,0);
    if(sem_w == SEM_FAILED || sem_r == SEM_FAILED)
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    char buf[256];
    while(1)
    {
        printf("P2请输入发给P1的消息：\n");
        fgets(buf,sizeof(buf),stdin);
        sem_wait(sem_w);
        strcpy(shm,buf);
        sem_post(sem_r);

        //如果输入quit就退出
        if(strcmp(shm,"quit\n")==0)
        {
            sem_wait(sem_w);
            break;
        }



    }
    sem_close(sem_w);
    sem_close(sem_r);
    shmdt(shm);
    // 删除共享内存
    shmctl(shmid, IPC_RMID, NULL);
    return 0;

}

