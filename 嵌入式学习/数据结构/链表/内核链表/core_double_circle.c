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
int main(int argc,char **argv)
{
    LIST_HEAD(main_list);
    LIST_HEAD(odd_list);
    LIST_HEAD(jdd_list);
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
    list_free_all(&main_list);
    
    return 0;
}
