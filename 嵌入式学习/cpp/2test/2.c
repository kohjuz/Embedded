#include <iostream>
using namespace std;

float f(float x, float y)
{
    if((x+y)==0)
    {
        throw "参数毫无意义";
    }
    else if(x==0||y==0)
    {
        //程序退出
        exit(0);
    }
    return (x*y)/(x+y);
}

int main()
{
    float x, y;
    while(1)
    {
        cout<<"请输入两个数:"<<endl;
        cin>>x>>y;
        try
        {
            cout<<"结果为:"<<f(x, y)<<endl;
        }
        catch(const char* msg)
        {
            cout<<msg<<endl;
        }
        
    }


    return 0;
}