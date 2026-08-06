#include<stdio.h>
int main()
{


    int h;
    char arr;
    printf("输入字符：");
    scanf("%c",&arr);
    printf("输入行数：");
    scanf("%d",&h);
 
    for(int i=1;i<=h;i++)
    {
        
          for(int k=1;k <= h-i;k++)
        {
            printf(" ");

        }
        for(int j=1;j<=(2*i)-1;j++)
        {
                printf("%c",arr);

        }
        printf("\n");
    }
  
    return 0;
}







