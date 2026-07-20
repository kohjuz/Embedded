#include<stdio.h>

int main()
{

    int a[3][2] = {{1,2},{3,4},{5,6}};
    int row = 3;
    int col = 2;

    
    int (*b[3])[col];

    
    for(int i = 0; i < row; i++)
    {
        b[i] = &a[i];
    }

    int sum = 0;
    for(int i = 0; i < row; i++)
    {
        
        for(int j = 0; j < col; j++)
        {
            sum += (*b[i])[j];
        }
    }

    printf("二维数组总和 = %d\n", sum);
    return 0;
}