#include<stdio.h>
struct daty
{
    int date;
    int month;
    int year;


    
    /* data */
};
void init_t(int year,int month,int date)
{
    int sum;

    struct daty S1={date,month,year};

    
    
    printf("%d年%d月%d日\n是一年中的第%d天",S1.year,S1.month,S1.date,sum);

}

void main()
{
    init_t(2026,1,1);


}