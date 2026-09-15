#include <iostream>

int a=100;
int &ra=a;//引用取别名，与a是同一块内存
//引用的同时要赋值

int b=20;

//引用
void add(int &a,int &b)
{
    int tmp;
    tmp=a;
    a=b;
    b=tmp;
}
//传地址
void add(int *a,int *b)
{
    int tmp=*a;
    *a=*b;
    *b=tmp;
}
using namespace std;
int main() 
{
    cout<<"a: "<<a<<endl;
    cout<<"b: "<< b<< endl;
    add(a,b);
    cout <<"第一次交换(引用)"<<endl;
    cout<<"a: "<<a<<endl;
    cout<<"b: "<< b<< endl;
    add(&a,&b);
    cout <<"第二次交换（地址）"<<endl;
    cout<<"a: "<<a<<endl;
    cout<<"b: "<< b<< endl;

    return 0;
}