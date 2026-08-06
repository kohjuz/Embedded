#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct node
{
    int data;
    struct node *next;
   
}node;


typedef struct 
{
    node *top;
    int size;   

}list_stack;

list_stack *initstack()
{
    list_stack *s=malloc(sizeof(list_stack));
    if(s==NULL)
    {
        return s;
    }
    else
    {
        s->top=NULL;
        s->size=0;
    }
    return s;
}
node *new_node(int data)
{
    node *new_node=calloc(1,sizeof(node));
    if(new_node!=NULL)
    {
        new_node->data=data;
        new_node->next=NULL;
    }
    return new_node;
}
bool push(list_stack *stack,node *new_node)
{
    new_node->next=stack->top;
    stack->top=new_node;
    stack->size++;
  
}
bool isEmpty(list_stack *stack)
{
    return stack->size==0;
}
node *top(list_stack *stack)
{
    if(isEmpty(stack))
    {
        return NULL;
    }
    return stack->top;
}


int main(int argc,char *argv[])
{
    list_stack *stack=initstack();
    if(stack)
    {
        printf("初始化栈空间成功\n");
    }
    push(stack,new_node(5));
    push(stack,new_node(4));

    printf("%d\n",top(stack)->data);



    return 0;
}