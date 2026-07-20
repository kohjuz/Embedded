#include<stdio.h>

int main()
{
    int m=3;//数组的3行
    int n=2;//数组的2行
    int a[3][2]={{1,2},{3,4},{5,6}};
    int (*b[3])[n];//定义一个b[3]数组，数组里面存3个指针，指针指向包含int [n]的一维数组
    for(int i=0;i<m;i++)//将b数组的每一个指针指向a
    {
        b[i]=&a[i];
    
    }
    int sum=0;
    for(int j=0;j<m;j++)//外循环用来将b[m]每一行输出
    {
        for(int k=0;k<n;k++)
        {
            sum+=(*b[j])[k];
        }

    }
    printf("数组和为：%d",sum);



    return 0;
}