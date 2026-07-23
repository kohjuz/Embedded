#include <stdio.h>
#include "list_test.h"

int main(void)
{
    LNode *head = list_init();
    int n;
    int ret=scanf("%d",&n);
    if(ret!=1)
    {
        printf("error 输入数据错误，输入的不是有效数字\n");
        while(getchar()!='\n');
        destroy(head);
        return -1;
    }
    
    for(int i=1; i<=n;i++)
    {
        LNode *new =new_node(i);

        
        add_node_tail(head,new);
    }
    show(head);
    show_tail(head);


    destroy(head);
    return 0;

}