#include<stdio.h>
static char n,ch;
char arr[26]={};
void sa(char a);

int main()
{
    printf("请输入一个大写字母:");
    scanf("%c",&n);
    int total = n - 'A' + 1;
    // p控制当前第几行
    for(int p=1;p<=total;p++)
    {
        // 前置空格
        for(int k=1;k <= total-p;k++)
        {
            printf(" ");
        }
        // 当场生成当前行的回文字符串
        char curChar = 'A' + p - 1;
        int idx = 0;
        for(int i='A';i<=curChar;i++)
        {
            arr[idx++]=i;
        }
        for(int i=curChar-1;i>='A';i--)
        {
            arr[idx++]=i;
        }
        arr[idx]='\0';
        // 打印本行
        for(int j=0;arr[j]!='\0';j++)
        {
            printf("%c",arr[j]);
        }
        printf("\n");
    }
    return 0;
}
// 若不需要预填充数组，空实现即可
void sa(char a){}