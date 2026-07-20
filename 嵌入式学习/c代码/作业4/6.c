
#include<stdio.h>
int main()
{

    int count;
    float b;
    st:
    int n=scanf("%d%f",&count,&b);
    printf("%d\n",n);
   
            if(n==2)
        {
            printf("n=%d,完成任务",n);
            
        }
        else if(n!=2)
        {
            printf("数据erro，请重新输入数据\n");
            char c;
                while(1)
                {
                c=getchar();
                if(c=='\n')
                break;

                }
            
                
            goto st;
       
        }
    return 0;
}
