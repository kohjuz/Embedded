#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>

// 将a去重之后放入b
void strip1(char a[], char b[])
{
    // 标记每个字符是否已经出现
    bool ch[255] = {0};

    // 遍历a中d的每个字符
    for(int i=0, k=0; a[i]!='\0'; i++)
    {
        // a[i]从未出现过
        if(ch[ a[i] ] == false)
        {
            b[k++] = a[i];
            ch[ a[i] ] = true;
        }

        // // 判定该字符是否存在于字符数组b中
        // if(strchr(b, a[i]) == NULL)
        // {
        //     b[k] = a[i];
        //     k++;
        // }
    }
}

void strip2(char a[])
{
    // 1. 将所有重复的字符标记出来
    int k=0, begin=0;

    for(int i=0; a[i]!='\0'; i++)
    {
        char ch = a[i];

        // 加入当前ch已经被标记过了,就字节跳过
        if(ch == -1)
            continue;

        // 将后续所有字符都跟ch对比,若重复就标记
        for(int j=i+1; a[j]!='\0'; j++)
        {
            if(a[j] == ch)
                a[j] = -1;
        }
    }

    // 找到k、begin的准确位置
    while(a[k++]!=-1);
    begin = --k;
    while(a[begin++]==-1);

    // 2. 从标记后的字符之后的第一个正常字符开始往前移
    for(int i=--begin; a[i]!='\0'; i++)
    {
        if(a[i] == -1)
            continue;

        a[k++] = a[i];
    }

    // 3. 最后补上\0
    a[k] = '\0';
}

int main(int argc, char const *argv[])
{
    char a[100];
    char b[100]; // 随机值

    printf("请输入一个字符串:\n");
    fgets(a, 100, stdin);

    // 去除重复字符
    // 对数组b清零
    bzero(b, 100);
    // memset(b, 0, 100);
    // strip1(a, b);
    strip2(a);

    // printf("去除重复字符之后：%s", b);
    printf("去除重复字符之后：%s", a);

    return 0;
}
