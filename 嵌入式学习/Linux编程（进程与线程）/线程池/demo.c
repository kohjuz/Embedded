#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

// 先写一个任务结点结构体，用来存放任务清单
typedef struct task
{
    void (*func)(void *arg);//任务要执行的函数
    void *arg;//任务函数的参数
    struct task *next;//链表指向下一个任务结点
}task_t;

//线程池结构体，也就是我招聘的人才库
typedef struct
{
    pthread_mutex_t mutex;//互斥锁，保护任务队列
    pthread_cond_t cond;//条件变量，用来睡觉，叫醒工人
    task_t *task_head;//任务队列的头结点,就是任务链表头
    int thread_num;//工人数量
    int shutdown;//是否销毁线程池 0正常运行，1销毁线程池
}threadpool_t;

//创建线程池，传入工人数量，返回线程池指针
threadpool_t *threadpool_create(int th_num);

    //工作线程，工人函数
    void *worker(void *arg);

    //往线程池添加任务
    int threadpool_add_task(threadpool_t *pool, void (*func)(void*), void *arg);
    
    //销毁线程池
    void threadpool_destroy(threadpool_t *pool);


threadpool_t *threadpool_create(int th_num)
{
    //1. 给人才库申请内存
    threadpool_t *pool = malloc(sizeof(threadpool_t));
    if(pool == NULL)
    {
        perror("malloc pool fail");
        return NULL;
    }

    //2.初始化锁、条件变量
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    //3.初始化成员
    pool->task_head = NULL;   //一开始没有任务
    pool->thread_num = th_num;
    pool->shutdown = 0;       //0：正常上班

    //4.循环招聘 th_num 个工人（创建线程）
    for(int i = 0; i < th_num; i++)
    {
        pthread_t tid;
        //工人函数worker，把人才库pool传给工人
        pthread_create(&tid, NULL, worker, pool);
        pthread_detach(tid);
    }

    return pool;
}

void *worker(void *arg)
{
    threadpool_t *pool = (threadpool_t*)arg;

    //工人无限循环，等待干活
    while(1)
    {
        //访问任务队列，第一件事：上锁
        pthread_mutex_lock(&pool->mutex);

        //❗没有任务，并且不下班，就睡觉 while！！
        while(pool->task_head == NULL && pool->shutdown == 0)
        {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }

        //被叫醒之后，如果shutdown等于1，下班走人
        if(pool->shutdown == 1)
        {
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(NULL);
        }

        //走到这里：有任务，取出第一个任务
        task_t *cur = pool->task_head;
        pool->task_head = pool->task_head->next;

        //拿到任务，立刻解锁！不要拿着锁干活！
        pthread_mutex_unlock(&pool->mutex);

        //执行任务
        cur->func(cur->arg);
        free(cur);
    }
    return NULL;
}

int threadpool_add_task(threadpool_t *pool, void (*func)(void*), void *arg)
{
    if(pool == NULL || pool->shutdown == 1)
    {
        return -1;
    }

    //新建一张任务纸条
    task_t *new_task = malloc(sizeof(task_t));
    new_task->func = func;
    new_task->arg = arg;
    new_task->next = NULL;

    //修改任务链表，必须上锁
    pthread_mutex_lock(&pool->mutex);

    //尾插，放到任务队列末尾
    if(pool->task_head == NULL)
    {
        pool->task_head = new_task;
    }
    else
    {
        task_t *p = pool->task_head;
        while(p->next != NULL)
        {
            p = p->next;
        }
        p->next = new_task;
    }

    pthread_cond_signal(&pool->cond); //叫醒一个工人
    pthread_mutex_unlock(&pool->mutex);

    return 0;
}

void threadpool_destroy(threadpool_t *pool)
{
    if(pool == NULL) return;

    pthread_mutex_lock(&pool->mutex);
    pool->shutdown = 1;                 //标记：下班
    pthread_cond_broadcast(&pool->cond); //叫醒所有睡觉工人
    pthread_mutex_unlock(&pool->mutex);

    sleep(1); //简单等待线程退出，作业这样写没问题

    //释放剩下没执行的任务
    task_t *p = pool->task_head;
    while(p != NULL)
    {
        task_t *tmp = p;
        p = p->next;
        free(tmp);
    }

    //销毁锁和条件变量
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);

    free(pool);
}

//测试任务
void mytask(void *arg)
{
    int num = *(int*)arg;
    printf("线程%lu处理任务 %d\n", pthread_self(), num);
    usleep(200000);
}

int main(void)
{
    //创建线程池，3个工人
    threadpool_t *pool = threadpool_create(3);

    //投放5个任务
    for(int i = 0; i < 5; i++)
    {
        int *p = malloc(sizeof(int));
        *p = i;
        threadpool_add_task(pool, mytask, p);
    }

    sleep(3);
    threadpool_destroy(pool);
    return 0;
}
