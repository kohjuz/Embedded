#include<stdio.h>
#include<stdlib.h>
//节点定义
typedef struct LNode
{
    int data;
    struct LNode *next;
}LNode;
//链表逆序输出
void printf_re(LNode *p)
{
    if(p==NULL)
    {
        return;
    }
    printf_re(p->next);
    printf("%d",p->data);
}

int main()
{

    LNode *L=(LNode *)malloc(sizeof(LNode));
    L->next=NULL;
    LNode *tail=L;
    int n;
    printf("请输入链表的长度: ");
    scanf("%d", &n);
    for(int i=0;i<n;i++)
    {
        LNode *s=(LNode *)malloc(sizeof(LNode));
        printf("请输入链表的第%d个元素: ", i+1);
        scanf("%d",&s->data);
        s->next=NULL;
        //尾插法
  
            tail->next=s;
            tail=s;
        
    }
   
    printf("链表正向输出：");
    for( LNode *p=L->next;p!=NULL;p=p->next)
    {
        printf("%d",p->data);
    }
    printf("\n");
    printf("逆向输出链表：");
    printf_re(L->next);
    printf("\n");




    return 0;
}
