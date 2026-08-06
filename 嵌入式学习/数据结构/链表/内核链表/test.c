#include <stdio.h>
#include <stdlib.h>
#include "kernel_list.h"
typedef struct node
{
    int data;
    struct list_head list;

}node;

struct list_head *node_new(int data)
{
    node *node_new = malloc(sizeof(node));
    if(node_new != NULL)
    {
        node_new->data = data;
        INIT_LIST_HEAD(&node_new->list);
    }
    return &node_new->list;
    
}


int main(int argc,char ** argv)
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if(head == NULL)
    {
        perror("malloc head fail");
        return -1;
    }
    INIT_LIST_HEAD(head);

    printf("ÊäÈëÊı×Ö:\n");
    int n;
    scanf("%d",&n);
    for(int i=1;i<=n;i++)
    {
        list_add_tail(node_new(i),head);
        
    }
    struct list_head *pos;
    list_for_each(pos,head)
    {
        printf("%d ",list_entry(pos,node,list)->data);
    }
    printf("\n");
    struct list_head *p=head->prev,*k=head;
    list_for_each_prev(p,head)
    {
        struct list_head *pre_tmp = p->prev;
        if(list_entry(p,node,list)->data%2==1)
        {
            k=p;
        }
        else
        {
            list_del(p);
            list_add_tail(p,k);
        }
        p = pre_tmp;
        
    }

    list_for_each(pos,head)
    {
        printf("%d ",list_entry(pos,node,list)->data);
    }
    printf("\n");


}