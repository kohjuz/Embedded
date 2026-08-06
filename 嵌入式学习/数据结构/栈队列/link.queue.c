#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



struct node 
{
    int data;
    struct node *next;
};
typedef struct linkqueue
{
    struct node *front;
    struct node *rear;
    int size;
}linkqueue;

linkqueue *init_q()
{
    linkqueue *q=(linkqueue *)malloc(sizeof(linkqueue));
    if(q!=NULL)
    {
        q->front=q->rear=NULL;
        q->size=0;
    }
    return q;
}
struct node *new_node(int data)
{
    struct node *n=(struct node *)malloc(sizeof(struct node));
    if(n!=NULL)
    {
        n->data=data;
        n->next=NULL;
    }
    return n;
}
void enqueue(linkqueue *q,struct node *n)
{
    if(q==NULL)
    {
        perror("队列为空:");
        return;
    }
    if(n==NULL)
    {
        perror("节点为空:");
        return;
    }
    if(q->front==NULL)
    {
        q->front=q->rear=n;
        q->size++;
    }
    else
    {
        q->rear->next=n;
        q->rear=n;
        q->size++;
    }
}
struct node *out_queue(linkqueue *q)
{
    if(q==NULL||q->front==NULL)
    return NULL;

    struct node *p;
    p=q->front;
    
    q->front=q->front->next;
    p->next=NULL;


    q->size--;

    if(q->size==0)
    {
       q->rear=NULL;
    }
    return p;
}
void destroy_q(linkqueue *q)
{
    if(q==NULL)
    {
        perror("队列为空:");
        return;
    }
    struct node *p;
    while((p=out_queue(q))!=NULL)
    {
        free(p);
    }
}
bool isEmpty(linkqueue *q)
{
    return q->size==0;
}

int main(int argc, char *argv[])
{
    //空队列
    linkqueue *q=init_q();
    if(q==NULL)
    {
        perror("初始化队列失败:");
        return -1;
    }
    //入队
    for(int i=0;i<10;i++)
    {
        enqueue(q,new_node(i));
    }

    //出队检查顺序
    while(!isEmpty(q))
    {
        struct node *p=out_queue(q);
        if(p==NULL)
        {
            perror("出队失败:");
            return -1;
        }
        printf("%d ",p->data);
        free(p);
        
    }
    printf("\n");

    //销毁队列

    free(q);
    return 0;
}
