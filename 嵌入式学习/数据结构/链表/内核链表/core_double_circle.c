#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel_list.h"

#ifndef DATATYPE 
#define DATATYPE int

#endif

typedef DATATYPE datatype;
//通用节点
typedef struct list_data
{
    datatype data;
    struct list_head list;
    
}list_data;
//创建节点
list_data *node_new(datatype val)
{
    list_data *node_new=(list_data *)malloc(sizeof(list_data));
    if(node_new==NULL)
    {
        perror("节点内存分配error");
        return NULL;
    }
    node_new->data=val;
    INIT_LIST_HEAD(&node_new->list);
    return node_new;
}
//遍历打印链表所有数据
void list_printf(struct list_head * head)
{
    if(list_empty(head))
    {
        perror("error");
        return;
    }
    list_data *p;
    list_for_each_entry(p,head,list)
    {
        printf("%d ",p->data);
    }
    printf("\n");

}
void list_free_all(struct list_head *head)
{
    list_data *pos, *tmp;
    list_for_each_entry_safe(pos, tmp, head, list)
    {
        list_del(&pos->list); // 先把节点从链表摘除
        free(pos);            // 再释放节点堆内存
    }
}
void list_reverse(struct list_head *head)
{
    if (list_empty(head) || head->next->next == head)
        return;

    struct list_head *cur = head->next;
    struct list_head *next;
    INIT_LIST_HEAD(head); // 清空原表头，重新头插实现反转

    while (cur != head)
    {
        next = cur->next;
        list_del(cur);
        list_add(cur, head); // 头插 = 逆序
        cur = next;
    }
}
int main(int argc,char **argv)
{
    LIST_HEAD(main_list);
    LIST_HEAD(odd_list);
    LIST_HEAD(even_list);
    int n;
    printf("请你输入任意自然数: (输入非数字退出)\n");
    while(1)
    {
        if(scanf("%d",&n)!=1)
        {
            while(getchar()!='\n');
            break;
        }
        list_data *new_node = node_new((datatype)n);
        if(new_node==NULL)return 0;
        list_add_tail(&new_node->list,&main_list);
    }
    printf("初始链表:");
    list_printf(&main_list);

     list_data *pos, *tmp;
    list_for_each_entry_safe(pos, tmp, &main_list, list)
    {
        list_del(&pos->list);
        if (pos->data %2==1)
        {
            list_add_tail(&pos->list, &odd_list);
        }
        else
        {
            list_add_tail(&pos->list, &even_list);
        }
    }

    // // 2. 偶数链表反转，变成降序
     list_reverse(&even_list);

    // 3. 拼接：奇数链表尾部接上偶数链表
    if (!list_empty(&even_list))
    {
        // 奇数链表尾节点的next 指向偶数链表第一个节点
        odd_list.prev->next = even_list.next;
        even_list.next->prev = odd_list.prev;
        // 偶数链表最后一个节点指向表头
        even_list.prev->next = &odd_list;
        odd_list.prev = even_list.prev;
        // 清空偶数表头（避免重复释放）
        INIT_LIST_HEAD(&even_list);
    }

    printf("奇偶重排后(奇数升序+偶数降序)：");
    list_printf(&odd_list);

    // 释放内存
    list_free_all(&odd_list);

    list_free_all(&main_list);
    return 0;
}

