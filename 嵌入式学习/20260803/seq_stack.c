#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct seq_stack
{
    int * stack;
    int size;
    int top;
}*seq_stack;

seq_stack init_stack(int size)
{
    seq_stack s=(seq_stack)malloc(sizeof(struct seq_stack));
    if(s!=NULL)
    {
        s->stack=malloc(size*sizeof(int));
        if(s->stack==NULL)
        {
            free(s);
            return NULL;
        }
    }
    s->size=size;
    s->top=-1;
    return s;
}
int is_full(seq_stack s)
{
    return s->top==s->size-1;
}
void push(seq_stack s,int x)
{
    if(is_full(s))
    {
        s->stack=realloc(s->stack,s->size*2*sizeof(int));
        return;
    }
    s->stack[++s->top]=x;
}
bool is_empty(seq_stack s)
{
    return s->top==-1;
}
int pop(seq_stack s)
{
    if(is_empty(s))
    {
        printf("栈空\n");
        return -1;
    }
    return s->stack[s->top--];
}
void free_stack(seq_stack s)
{
    if(s==NULL)
    {
        return;
    }
    free(s->stack);
    free(s);
}

int main(void)
{
    //空栈
    seq_stack s=init_stack(10);
    //依次入栈n个元素
    int n;
    scanf("%d",&n);
    for(int i=0;i<n;i++)
    {
       push(s,i);
    }
    //依次出栈
    while(!is_empty(s))
    {
        printf("%d\n",pop(s));
    }
    //释放栈

    free_stack(s);


}