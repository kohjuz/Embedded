#include<stdio.h>
void main()
{
    float f;
    printf("请输入华氏温度：");
    scanf("%f",&f);
    printf("摄氏温度%f",(5*(f-32))/9);


}