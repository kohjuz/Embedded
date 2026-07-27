#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum retcode {number, operator};

enum retcode parse(char **pp, char *pop, float *px)
{
    while((*pp)[0] == ' ') (*pp)++;
    char c = (*pp)[0];

    if((c == '+') || (c=='-') || (c=='*') || (c=='/'))
    {
        *pop = c;
        (*pp)++;
        return operator;
    }
    else
    {
        char *start = *pp;
        for(; ((*pp)[0]>='0' && (*pp)[0]<='9') || (*pp)[0]=='.'; (*pp)++);
        *px = atof(start);
        return number;
    }
}
bool judge(char op,char cur_stack2_op)
{
    bool op_high = (op == '*' || op == '/');
    bool stack_high = (cur_stack2_op == '*' || cur_stack2_op == '/');
    return op_high && !stack_high;
}
float operation(float a,char cur_stack2_op,float b)
{
    switch (cur_stack2_op)
    {
    case '+': return a+b; break;
    case '-': return a-b; break;
    case '*': return a*b; break;
    case '/': 
        if(b == 0)
        {
            printf("除数不能为0！\n");
            exit(-1);
        }
        return a/b; break;
    default:
        printf("非法运算符\n");
        exit(-1);
    }
    return 0.0f;
}

int main(int argc, char const *argv[])
{
    char buf[100];
    fgets(buf, 100, stdin);
    buf[strcspn(buf, "\n")]='\0';

    float stack1[20];
    int top1= -1;
    char  stack2[20];
    int top2= -1;

    float x;
    char op;
    char *p = buf;
    while(p!=NULL && p[0]!='\0')
    {
        if(parse(&p, &op, &x) == number)
        {
            printf("%f\t", x);
            stack1[++top1] =x;
        }
        else
        {
             while(1)
            {
                printf("%c\t", op);
                if(top2==-1 || judge(op,stack2[top2]))
                {
                    stack2[++top2]= op;
                    break;
                }
                else
                {
                    float a,b;
                    b=stack1[top1--];
                    a=stack1[top1--];
                    stack1[++top1]=operation(a,stack2[top2--],b);
                }
            }
        }
    }

    while(top2 != -1)
    {
        float b = stack1[top1--];
        float a = stack1[top1--];
        float res = operation(a, stack2[top2--], b);
        stack1[++top1] = res;
    }
    printf("计算结果： %f\n", stack1[top1]);

    return 0;
}