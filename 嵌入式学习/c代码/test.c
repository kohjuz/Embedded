#include <stdio.h>

int main()
{
    float fah;
    float cel;
    printf("请输入华氏温度:\n");
    while(1)
    {
        int ret = scanf("%f", &fah);
        if(ret == 1)
        {
            
            char ch;
            ch = getchar();
            if(ch == '\n')
            {
                break;
            }
            else
            {
                
                while(getchar() != '\n');
                printf("输入格式错误，请仅输入数字\n");
            }
        }
        else
        {
        
            while(getchar() != '\n');
            printf("输入格式错误，请仅输入数字\n");
        }
    }
    
    cel = (5 * (fah - 32)) / 9;
    printf("华氏%.2f° = 摄氏%.2f°\n", fah, cel);
    return 0;
}