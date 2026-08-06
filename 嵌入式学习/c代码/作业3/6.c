#include <stdio.h>
#include <stdbool.h>


int main(void)
{
    int num;
    printf("请输入一个十进制数: ");
    scanf("%d", &num); // 为了突出重点，此处未进行输入合法性检测，望读者知悉
    int i,a;
    char hex[10];
    if(num<0)
    {
        a=num;
        num=-num;
    
    }
    else
    {
        num=num;
    }
    for(i=0; i<10 && num!=0; i++)
    {
        switch(num % 16)
        {
        case 0 ... 9:
            hex[i] = num%16 + '0';
            break;
        case 10 ... 15:
            hex[i] = num%16 - 10 + 'A';
            break;
        }
        num /= 16;
    }
    if(a>0)
    {printf("");
    }
    else
    {
        printf("-");
    }


    
  
    
    // 将数组num中的数字倒序输出
    int j;
    for(j=i-1; j>=0; j--)
    {
        printf("%c", hex[j]);
    }
    
    printf("\n");
    return 0;
}