#include <stdio.h>
char *p="adbccadaaf";
char *k="edbccadrff";
char str[100];


int main()
{
    int max_len=0;
    int start=0;
    
    for(int j=0;k[j]!='\0';j++)
    {   int len=0;
        for(int i=0;p[i]!='\0';i++)
        {
           
            while(*(p+i+len)!='\0'&&*(k+j+len)!='\0'&&*(p+i+len)==*(k+j+len))
            {
                len++;
            }
            if(len>max_len)
        {
            max_len=len;
            start=i;
        }
            
        }

    }

    for (int m = 0; m < max_len; m++)
    {
        str[m] = p[start + m];
    }
    str[max_len] = '\0'; 

    printf("最长公共连续子串：%s\n", str);
}
// #include<stdio.h>

// int a;
// int b=1;
// int main()
// {
// printf("请输入一个小端序数据:");
// scanf("%d",&a);
// for(int i=0;i<8;i++)
// {
// b=a&(0x00000001<<i);
// printf("%#x\n",b);
// b=b<<(i+1);

// }

// printf("%#x\n",b);

// }
// #include<stdio.h>

// unsigned int little2big(unsigned int v)
// {
//     return (v<<24)|((v<<8)&0xFF0000)|((v>>8)&0xFF00)|(v>>24);
// }

// int main()
// {
//     unsigned int a;
//     scanf("%x",&a);
//     printf("0x%08x",little2big(a));
//     return 0;
// }