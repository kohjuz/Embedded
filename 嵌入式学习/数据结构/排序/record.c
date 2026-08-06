#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
// 最大参赛人数 题目限制 N≤100
#define MAX_NUM 100
// 姓名最大长度
#define NAME_LEN 32


// 选手结构体：存储姓名+完成用时（秒）
typedef struct Runner
{
    char name[NAME_LEN];  // 选手姓名
    int time;             // 跑步用时，单位秒
} Runner;

/**
 * @brief 冒泡排序：按跑步时间从小到大升序
 *        用时越少排名越靠前
 * @param arr 选手结构体数组
 * @param n 总人数
 */
void sortRunner(Runner arr[], int n)
{
    int i, j;
    Runner temp; // 交换临时变量
    // 外层循环：排序轮次
    for (i = 0; i < n - 1; i++)
    {
        // 内层循环：相邻两个选手比较
        for (j = 0; j < n - 1 - i; j++)
        {
            // 前一个选手用时 > 后一个，交换位置（升序）
            if (arr[j].time > arr[j+1].time)
            {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}
int readFile(Runner arr[])
{
    // 打开只读文件 record.txt
    FILE *fp = fopen("record.txt", "r");
    if (fp == NULL)
    {
        perror("打开record.txt失败");
        return -1;
    }

    int count = 0;
    // 循环读取每行：姓名 + 时间，读到文件末尾EOF结束
    while (fscanf(fp, "%s %d", arr[count].name, &arr[count].time) == 2)
    {
        count++;
        // 限制最大人数100，防止数组越界
        if (count >= MAX_NUM)
            break;
    }
    fclose(fp); // 关闭文件
    return count;
}

int main(void)
{
    srand((unsigned int)time(NULL));
    
    Runner runnerArr[MAX_NUM];
    int N;

    // 1. 读取文件中所有选手数据
    N = readFile(runnerArr);
    if (N <= 0)
    {
        printf("文件无有效数据，程序退出\n");
        return 1;
    }
    printf("文件中共读取到 %d 名选手\n", N);
    int num=rand()%N;
    printf("抽到的k为：%d\n",num);

 
    // 合法性校验K
    if (num < 1 || num > N)
    {
        printf("输入的K超出范围！\n");
        return 1;
    }

    // 3. 按跑步时间升序排序
    sortRunner(runnerArr, N);
    printf("第%d名选手姓名：%s  跑步成绩为：%d\n", num, runnerArr[num-1].name,runnerArr[num-1].time);

    return 0;
}