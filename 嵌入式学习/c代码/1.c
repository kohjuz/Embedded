#include <stdio.h>

int main() {
    int n, i, j;

    printf("请输入金字塔的层数: ");
    scanf("%d", &n);

    for (i = 1; i <= n; i++) {
        // 1. 打印前导空格 (为了美观，空格数随行数减少)
        for (j = 1; j <= n - i; j++) {
            printf("  "); // 两个空格，因为下面数字占位较宽
        
        }
        // 2. 打印递增数字 (1 到 i)
        for (j = 1; j <= i; j++) {
            printf("%-2d", j); // %-2d 表示左对齐，占2位，使图形更整齐
        }

        // 3. 打印递减数字 (i-1 到 1)
        for (j = i - 1; j >= 1; j--) {
            printf("%-2d", j);
        }

        // 4. 换行
        printf("\n");
    
    }
    return 0;
}
