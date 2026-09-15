#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

#include "shm_sem.h"

int main(void)
{
    /* 消费者不创建对象，直接打开（所以必须先启动 P1） */
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) { perror("shm_open"); return 1; }
    struct shm_data *shm = mmap(NULL, sizeof(struct shm_data),
                                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) { perror("mmap"); return 1; }

    sem_t *sem_empty = sem_open(SEM_EMPTY, 0);
    sem_t *sem_full  = sem_open(SEM_FULL,  0);
    sem_t *sem_mutex = sem_open(SEM_MUTEX, 0);
    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open"); return 1;
    }

    printf("[P2 消费者] 等待 P1 的数据...\n");
    while (1) {
        sem_wait(sem_full);              /* 等“有数据”，没数据就阻塞 */
        sem_wait(sem_mutex);             /* 进入临界区，保护 out 和 buf */

        printf("P2 收到：%s\n", shm->buf[shm->out]);
        if (strcmp(shm->buf[shm->out], "quit") == 0) {
            sem_post(sem_mutex);
            break;                        /* 收到退出哨兵，结束 */
        }
        shm->out = (shm->out + 1) % BUF_SLOTS;

        sem_post(sem_mutex);             /* 离开临界区 */
        sem_post(sem_empty);             /* 通知生产者“空出 1 个槽” */
    }

    /* 最后退出的进程负责清理内核对象，避免泄漏 */
    munmap(shm, sizeof(struct shm_data));
    close(fd);
    sem_close(sem_empty); sem_close(sem_full); sem_close(sem_mutex);
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_EMPTY); sem_unlink(SEM_FULL); sem_unlink(SEM_MUTEX);
    printf("P2 退出，已清理共享内存与信号量\n");
    return 0;
}
