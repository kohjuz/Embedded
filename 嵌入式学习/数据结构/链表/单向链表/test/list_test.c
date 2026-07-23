#include <stdio.h>
#include "list_test.h"

LNode *list_init(void)
{
    LNode *head=(LNode *)malloc(sizeof(LNode));
    if(head==NULL)
    {
        printf("头节点分配内存失败\n");
       
    }
    head->next=NULL;
    
    return head;

}
LNode *new_node(int data)
{
    LNode *new_node=(LNode *)malloc(sizeof(LNode));
    if(new_node==NULL)
    {
        printf("内存不足新建节点失败\n");
       
        return NULL;
    }
    new_node->data=data;
    return new_node;
}
void add_node(LNode *head,LNode *node)
{
    if(head==NULL||node==NULL)
    {
        return;
    }
    node->next=head->next;
    head->next=node;

}
void add_node_tail(LNode *head,LNode *node)
{
    LNode *tail=head;
    while(tail->next!=NULL)
    {
        tail=tail->next;
    }
    tail->next=node;
    node->next=NULL;

}

void show(LNode *head)
{
       if(head->next==NULL)
   {
        printf("链表为空\n");

   }
   else
   {
        printf("链表正向输出：");
        for(LNode *p=head->next;p!=NULL;p=p->next)
        {
            printf("%d ",p->data);
        }
        printf("\n");

   }
}
void reverse_list(LNode *head)
{
    LNode * next=NULL;
    LNode *pre=NULL;
    LNode *cur=head->next;
    while(cur!=NULL)
    {
        next=cur->next;
        cur->next=pre;
        pre=cur;
        cur=next;

        
    }
    head->next=pre;

}
void show_tail(LNode *head)
{
     printf("逆向输出链表：");
    reverse_list(head);
    for(LNode *p=head->next;p!=NULL;p=p->next)
    {
        printf("%d ",p->data);
    }
    printf("\n");

}

void destroy(LNode *head)
{
    LNode *cur = head;
    LNode *next_ptr=NULL;
    while(cur!=NULL)
    {
        next_ptr=cur->next;
        free(cur);
        cur=next_ptr;

    }
}