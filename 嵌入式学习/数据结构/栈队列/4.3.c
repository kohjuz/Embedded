#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void f(int n,int cur,int *input,int top,int *stack,int out,int output[])
{
    if(n==out)
    {
        for(int i=0;i<n;i++)
        printf("%d ",output[i]);
        printf("\n");
        return;
    }

    if(cur<n)
    {
        int val =stack[top];
        stack[++top]=input[cur];
        f(n,cur+1,input,top,stack,out,output);
        stack[--top]=val;
    }
    if(top>=0)
    {
        int val = stack[top];
        output[out]=stack[top];
        top--;
        f(n,cur,input,top,stack,out+1,output);
        stack[++top]=val;
    }

}
int main (void)
{

    int n;
    scanf("%d",&n);

    int input[n],output[n];
    for(int i=0;i<n;i++)
    {
        input[i]=i+1;
        
        
    }
    int stack[n];
    int top =-1;

    f(n,/*车厢数量*/
        0,/*当前即将入栈车厢标号*/
        input,/*进栈车厢数组*/
        top,/*栈顶位置*/
        stack,
        0,/*当前出栈车厢数*/
        output/*出栈车厢数组*/
    );


}