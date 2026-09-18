#include <iostream>

using namespace std;

class Time
{
    int h=0,m=0,s=0;
    bool Flase;
    public:
    int day=0;
    Time(){};
    Time(int _h,int _m,int _s):h(_h),m(_m),s(_s)
    {
        cout<<"当前时间为："<<h<<"时"<<m<<"分"<<s<<"秒"<<endl;
    }
    Time(int _h,int _m,int _s,int _day):h(_h),m(_m),s(_s),day(_day)
    {
        if(day)
        {
            cout<<"相加后时间为："<<"第"<<day<<"天"<<h<<"时"<<m<<"分"<<s<<"秒"<<endl;
        }
    }
    Time operator+(const Time &p)
    {
        int hour=(this->h + p.h);
        int min=(this->m + p.m);
        int second=(this->s + p.s);
        int _day=0;
        if(second>=60)
        {
            min+=(second/60);
            second=second%60;
        }
        if(min>=60)
        {
            hour+=min/60;
            min=min%60;
        }
        if(hour>=24)
        {
            _day=hour/24;
            hour=hour%24;
        }
        Time add(hour,min,second,_day);
        return add;
    }
};

int main()
{
    Time t1(23,48,32);
    Time t2(22,7,12);
    Time t3;
    t3=t1+t2;
    Time t4(24,60,60);
    Time t5(24,20,60);
    t3=t4+t5;
    return 0;
}