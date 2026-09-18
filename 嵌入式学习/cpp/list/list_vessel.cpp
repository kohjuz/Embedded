#include <iostream>
#include <list>
using namespace std;

//仿函数
//第一种：重载圆括号 
//第二种：谓词函数 一元谓词：返回值bool类型   形参 1个
//                二元谓词：返回值bool类型   形参 2个
//第三种
/*
    Lambda表达式（匿名函数）
    没有名字的函数：即用即弃  临时函数

    返回值[](){函数源码}
    [捕获列表](参数列表)->返回值类型 {函数体}
    l1.remove_if([](int n)->bool{if(n>=60&&n<=100) return true;elsereturn false;})

    捕获列表：捕获外部变量 （能访问变量）要修改要加mutable且修改的也只是复制的副本
    [=]：全部捕获 [&]:全部捕获引用
    [=,&x]:捕获除&x以外的所有值
    [=,y]::捕获除y以外的所有值
    全局作用域的 lambda，不能使用默认捕获`[=]`或者`[&]`
    要把lambda函数写在main里才可以用

*/

class odd
{
public:
    bool operator()(int n)
    {
        if(n%2==1)
        return true;
        else
        return false;
    }
};
//谓词函数
bool f(int n)
{
    if(n>=60&&n<=100)
    return true;
    else
    return false;
}
//lambda
int a=571;
int b=437;
int main()
{
    list<int>l1={10,20,30,43,50,61,70,89,99};
    //remove 删除元素
    //l1.remove(10);

    odd fun;
    cout<<"容器list：";
    for(auto x:l1)
    {
        cout<<x<<" ";
    }
    cout<<endl;

    //remove_if（仿函数）删除函数内所有奇数
    //l1.remove_if(fun);
    l1.remove_if(f);

    //cout<<"删除奇数后：";
    cout<<"删除大于60且小于100的: ";
    for(auto x:l1)
    {
        cout<<x<<" ";
    }
    cout<<endl;
    auto fun1=[=](int n)->void{cout<<"n为："<<n<<" "<<a<<" "<<b<<endl;};
    fun1(888);
    return 0;
}