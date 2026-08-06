#include<stdio.h>
#include<stdlib.h>

#include<stdbool.h>

#define MAX_NODE_COUNT 1000 //最大节点限制宏
//节点定义
typedef struct LNode
{
    int data;
    struct LNode *next;
}LNode;
//销毁链表，释放内存
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
//3指针反转链表
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

//链表逆序输出，递归
void printf_re(LNode *p)
{
    if(p==NULL)
    {
        return;
    }
    printf_re(p->next);
    printf("%d",p->data);
}
//数组存数据，逆序输出
void printf_reverse_safe(LNode * head)
{
    int strc[MAX_NODE_COUNT];
    int n=-1;
    LNode *cur=head->next;
    while(cur!=NULL)
    {
        strc[++n]=cur->data;
        cur=cur->next;

    }
    while(n>=0)
    {
        printf("%d ",strc[n--]);
    }
    printf("\n");
}

int main()
{

    LNode *head=(LNode *)malloc(sizeof(LNode));
    if(head==NULL)
    {
        printf("头节点内存分配失败,程序退出\n");
        return -1;
    }
    head->next=NULL;
    LNode *tail=head;
    int n;
    printf("请输入链表的长度: ");
    int ret = scanf("%d", &n);
    if(ret!=1)
    {
        printf("error 输入数据错误，输入的不是有效数字\n");
        while(getchar()!='\n');
        destroy(head);
        return -1;
    }
    if(n<=0||n>MAX_NODE_COUNT)
    {
        printf("数字n不在有效范围0~%d内",MAX_NODE_COUNT);
        while(getchar()!='\n');
        destroy(head);
        return -1;
    }
    while(getchar()!='\n');
    for(int i=0;i<n;i++)
    {
        LNode *new_node=(LNode *)malloc(sizeof(LNode));
        if(new_node==NULL)
        {
            printf("内存不足新建节点失败\n");
            destroy(head);
            return -1;
        }
        printf("请输入链表的第%d个元素: ", i+1);
        int res=scanf("%d",&new_node->data);
        if(res!=1)
        {
            printf("输入数据error\n");
            while(getchar()!='\n');
            destroy(head);
            return -1;
        }
         while(getchar()!='\n');
        new_node->next=NULL;
        //尾插法
  
            tail->next=new_node;
            tail=new_node;
        
    }
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

   }

    printf("\n");
    printf("逆向输出链表：");
    reverse_list(head);//3指针翻转链表

        for(LNode *p=head->next;p;p=p->next)
        {
            printf("%d ",p->data);
        }
    reverse_list(head);    //恢复链表
    // printf_reverse_safe(head);//数组存储逆向输出
    printf("\n");



   destroy(head);
    return 0;
}
