#include <iostream>

using namespace std;
/*
    const_cast<char *>(a)去除指针或者引用的const或者volatile属性
*/

int main()
{
    int i=6;
    const int &ri=i;
    //ri=8; //因为const
    //去掉const属性
    const_cast<int &>(ri)=8;

    cout<<ri<<endl;

    int num=100;
    const int *p=&num;
    //*p=200;//不可以
    *(const_cast<int * >(p))=200;
    cout<<num<<endl;

    //int *k=p;//不可以因为类型不匹配p时const int *
    int *k=const_cast<int *>(p);
    cout<<*k<<endl;

    //static_cast  不允许两个完全无关的类型进行转换
    //静态转换,转换在编译阶段，不在程序运行阶段

    //不改内存里面的东西，只是告诉编译器，把这一块内存当成这个类型来读
    float *pf=reinterpret_cast<float*>(&i);
    cout<<*pf<<endl;

    

    return 0;
}