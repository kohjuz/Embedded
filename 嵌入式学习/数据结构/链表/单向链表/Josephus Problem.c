#include <stdio.h>
#include<stdlib.h>
#define MAX_N 1000
typedef struct LNode
{
    int data;
    struct LNode *next;
}LNode;

void destroy(LNode *head)
{
    LNode *cur=head;
    LNode *prt=NULL;
    while(cur!=NULL)
    {
        prt=cur->next;
        free(cur);
        cur=prt;
    }

}

 
 





int main()
{
    LNode *head=(LNode *)malloc(sizeof(LNode));
    if(head==NULL)
    {
        printf("头节点内存分配失败\n");
        destroy(head);
        return -1;
    }
    int n=0;
    printf("请输入人数：\n");
    int ret=scanf("%d",&n);
    if(ret!=1)
    {
        printf("输入数据error，请输入数字\n");
        while(getchar()!='\n');
        destroy(head);
        return -1;
    }
    if(n<2)
    {
        printf("输入人数等于%d,游戏无法进行\n",n);
        while(getchar()!='\n');
        destroy(head);
        return -1;
    }
    while(getchar()!='\n');
    int arr[MAX_N];
    for(int i=0;i<n;i++)
    {
        
        arr[i]=i+1;
    }
        
        head->next=NULL;
        LNode *tail=head;
    for(int j=0;j<n;j++)
    {

        LNode *new_node=(LNode *)malloc(sizeof(LNode));
            if(new_node==NULL)
            {
                printf("新建节点内存分配失败\n");
                destroy(head);
                return -1;
            }
        new_node->data=arr[j];
        new_node->next=NULL;
        tail->next=new_node;
        tail=new_node;
 
    }
    tail->next = head->next;
            printf("有节点：");
            LNode *start=head->next;
            LNode *p=start;
            int count=0;
        do
        {
            printf("%d ",p->data);
            p=p->next;
           

        } while(p!=start);
        printf("\n");
            LNode *pre=head;
            LNode *cur_dd=head->next;        
        while(n>2)
        {
           
            count++;

            if(count == 3)
            {   static int num;
                ++num;
                count=0;
                printf("进入第%d次循环\n",num);
                
                pre->next=cur_dd->next;
                LNode *next=cur_dd;
                printf("删除节点%d\n",cur_dd->data);
                cur_dd=cur_dd->next;
                
                
                
                free(next);
                n--;
                
                
            }
            else
            {
                pre=cur_dd;
                cur_dd=cur_dd->next;
            }
            


        }
        LNode *temp=cur_dd;
        printf("存活节点：");
        do
        {
            printf(" %d",temp->data);
            temp=temp->next;
            

        }while(temp!=cur_dd);
        printf("\n");


    destroy(head);

    return 0;
}