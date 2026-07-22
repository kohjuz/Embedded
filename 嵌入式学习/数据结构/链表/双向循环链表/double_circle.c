#include<stdio.h>
#include <stdlib.h>


typedef struct LNode
{
    int data;

    struct LNode *next;
    struct LNode *prev;
    

}LNode;

LNode * init_head()
{
    LNode *head=(LNode *)malloc(sizeof(LNode));
    if(head!=NULL)
    {
        head->next=head;
        head->prev=head;

    }
    return head;

}
LNode *new_node(int data)
{
    LNode *new=(LNode *)malloc(sizeof(LNode));
    if(new!=NULL)
    {
        new->data=data;
        new->next=new;
        new->prev=new;
    }
    return new;

}
void init_list(LNode * head,LNode *new)
{
    new->prev=head->prev;
    new->next=head;
    head->prev->next=new;
    head->prev=new;

}
void show(LNode *head)
{
    for(LNode *p=head->next;p!=head;p=p->next)
    {
        printf("%d ",p->data);
    }
    printf("\n");
}
void odd_int(LNode *head)
{
    LNode *s=head->next;
    while(s!=head)
    {
        if((s->data)%2!=0)
        {
            // s->next=s->next->next;
            printf("%d ",s->data);
            
            
        }
        s=s->next;

        
    }

}
void even_int(LNode *head)
{
    LNode *s=head->next;
    while(s!=head)
    {
        if((s->data)%2==0)
        {
            // s->next=s->next->next;
            printf("%d ",s->data);
            
            
        }
        s=s->next;

        
    }
    
}


int len=0;

int main(int argc,char **argv)
{
    LNode * head=init_head();
    if(head)
    {
        printf("初始化链表成功\n");
    }
    else
    {
        perror("初始化失败");
        return -1;
    }
    printf("请输入自然数:");
    scanf("%d",&len);
    
    for(int i=1;i<=len;i++)
    {
        LNode * new=new_node(i);
        init_list(head,new);
    }
    show(head);
    odd_int(head);
    even_int(head);

    return 0;
}