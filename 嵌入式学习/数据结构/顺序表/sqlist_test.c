#include <stdio.h>
#include "sqlist.h"

int main(int argc, char const *argv[])
{
    // 1. 搞个空顺序表
    sqlist *sql = sqlist_init(10);

    // 2. 将若干个整数放入顺序表
    printf("请输入若干个整数（输入非整数就退出）\n");
    int n;
    while(1)
    {
        if(scanf("%d", &n) != 1)
            break;
        add_data(sql, n);
    }

    // 排序
    sort(sql);

    // 3. 将数据正、倒序输出
    output(sql);
    reverse_output(sql);

    // 4. 销毁顺序表
    restroy(sql);

    return 0;
}
