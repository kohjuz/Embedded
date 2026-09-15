#include <iostream>
#include <iomanip>
using namespace std;
//进制输出
//十进制dec
//八进制oct
//十六进制hex（小写字母）
//hex<<uppercase(大写字母)

//浮点小数点位数
//fixed<<setprecision 保留几位小数

//cin.fail() 输入类型不匹配，读取失败后续都读取失败
//cin.clear() 清空失败标志位
//cin.ignore() 清空输入缓存李残留的错误字符
int main()
{
    int a=100;
    int b=20;
    cout <<"输出十进制"<<a<<" "<<b<<endl;
    cout <<"输出八进制"<<oct<<a<<" "<<oct<<b<<endl;
    cout <<"输出十六进制小写"<<hex<<a<<" "<<hex<<b<<endl;
    cout <<"输出十六进制大写"<<hex<<uppercase<<a<<" "<<hex<<uppercase<<b<<endl;  
    //恢复十进制输出
    cout <<dec;
    //浮点数输出
    double c=3.1415926;
    cout <<"输出浮点数"<<c<<endl;
    cout <<"输出浮点数保留两位小数"<<fixed<<setprecision(2)<<c<<endl;
    cout <<"输出浮点数保留四位小数"<<fixed<<setprecision(4)<<c<<endl;
    cout <<"保留有效位数"<<setprecision(3)<<c<<endl;
    //判断cin输入是否符合要求
    int num;
    if(!(cin>>num))
    {
        cout <<"输入类型不匹配"<<endl;
        cin.clear();
        cin.ignore(1024,'\n');
    }
    else
    {
        cout <<"输入的数字是"<<num<<endl;
    }
    return 0;
}