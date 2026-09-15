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
    /* 1) 创建/打开共享内存对象，并映射到本进程地址空间 */
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) { perror("shm_open"); return 1; }
    if (ftruncate(fd, sizeof(struct shm_data)) == -1) { perror("ftruncate"); return 1; }
    struct shm_data *shm = mmap(NULL, sizeof(struct shm_data),
                                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) { perror("mmap"); return 1; }

    /* 2) 打开三个 POSIX 信号量（生产者负责创建，初值很关键） */
    sem_t *sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, BUF_SLOTS); /* 空槽数 = 容量 */
    sem_t *sem_full  = sem_open(SEM_FULL,  O_CREAT, 0666, 0);         /* 已填槽数 = 0 */
    sem_t *sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);         /* 互斥锁 = 1 */
    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open"); return 1;
    }

    char line[MSG_SIZE];
    printf("[P1 生产者] 输入要发给 P2 的内容，输入 quit 结束\n");
    while (1) {
        printf("P1> "); fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = '\0';

        sem_wait(sem_empty);             /* 等“有空槽”，满了就阻塞 */
        sem_wait(sem_mutex);             /* 进入临界区，保护 in 和 buf */

        strncpy(shm->buf[shm->in], line, MSG_SIZE - 1);
        shm->buf[shm->in][MSG_SIZE - 1] = '\0';
        shm->in = (shm->in + 1) % BUF_SLOTS;

        sem_post(sem_mutex);             /* 离开临界区 */
        sem_post(sem_full);              /* 通知消费者“多了 1 条数据” */

        if (strcmp(line, "quit") == 0) break;
    }

    /* 关闭本进程句柄（不 unlink，对象由最后退出的 P2 清理） */
    munmap(shm, sizeof(struct shm_data));
    close(fd);
    sem_close(sem_empty); sem_close(sem_full); sem_close(sem_mutex);
    return 0;
}
