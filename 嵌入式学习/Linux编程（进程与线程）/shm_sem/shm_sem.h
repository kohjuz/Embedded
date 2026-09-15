#ifndef SHM_SEM_H
#define SHM_SEM_H

#include <stddef.h>

/* 共享内存与信号量的名字（必须以 '/' 开头，内核对象在 /dev/shm 下） */
#define SHM_NAME   "/my_shm_p1p2"
#define SEM_EMPTY  "/sem_empty_p1p2"
#define SEM_FULL   "/sem_full_p1p2"
#define SEM_MUTEX  "/sem_mutex_p1p2"

/* 环形缓冲区容量（槽位数）与单条消息长度 */
#define BUF_SLOTS  4
#define MSG_SIZE   1024

/* 放进共享内存的结构体：生产者和消费者各持一份 mmap 映射到同一块物理内存 */
struct shm_data {
    int in;                          /* 生产者下一个写入位置 */
    int out;                         /* 消费者下一个读取位置 */
    char buf[BUF_SLOTS][MSG_SIZE];  /* 数据缓冲区 */
};

#endif /* SHM_SEM_H */
