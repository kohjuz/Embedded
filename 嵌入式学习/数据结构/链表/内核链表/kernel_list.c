#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "kernel_list.h"
struct studen
{
    char name[10];
    int age;
    int score;
    char sex[5];
    
};

// 大结构体
struct node
{
    struct  studen data;
    struct list_head list; // 小结构体
};
void show_studen(struct node *p)
{
    printf("姓名:%s\t年龄:%d\t成绩:%d\t性别:%s\n", p->data.name, p->data.age,p->data.score,p->data.sex);
}


struct node *init_list(void)
{
    // 内核链表是由头节点
    struct node *head =malloc(sizeof(struct node));
    if(head != NULL)
    {
        INIT_LIST_HEAD(&head->list);
    }
    return head;
}

void show_list(struct node *head,void (*func)(struct node *p))
{
    struct list_head *pos;
    list_for_each_prev(pos, &head->list)
    {
        struct node *p = list_entry(pos, struct node, list);
        func(p);
    }
    printf("\n");
}




int main(int argc, char const *argv[])
{
    struct node *head = init_list();

    
    
    

        struct node *n = malloc(sizeof(struct node));
        strcpy(n->data.name, "施炜杰");
        n->data.age = 21;
        n->data.score = 100;
        strcpy(n->data.sex,"男");
        list_add_tail(&n->list, &head->list);

        struct node *n2 = malloc(sizeof(struct node));
        strcpy(n2->data.name, "陈善杰");
        n2->data.age = 99;
        n2->data.score = 90;
        strcpy(n2->data.sex,"男");
        list_add_tail(&n2->list, &head->list);

        struct node *n3 = malloc(sizeof(struct node));
        strcpy(n3->data.name, "二次元梁振廷");
        n3->data.age = -100000;
        n3->data.score = 0; 
        strcpy(n3->data.sex,"二次元");
        list_add_tail(&n3->list, &head->list);

        struct node *n4 = malloc(sizeof(struct node));
        strcpy(n4->data.name, "王乾宇唐蛋");
        n4->data.age = 0;
        n4->data.score = 0;
        strcpy(n4->data.sex,"蛋");
        list_add_tail(&n4->list, &head->list);
    
    show_list(head,show_studen);
    
    struct list_head *pos, *num;
    list_for_each_safe(pos, num, &head->list)
    {
        list_del(pos);
        free(list_entry(pos,struct node,list));

    }


    return 0;
}
