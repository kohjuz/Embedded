#include <iostream>
#include <cstring>

using namespace std;


/*
    malloc 是库函数
    new 是关键字
    重要的是new 会调用构造函数
    delete 会调用构析函数
*/

int main()
{
    //对比申请堆空间c语言和c++

    //c语言申请堆空间
    int *n=(int *)malloc(sizeof(int));
    //c++申请堆空间
    int *n1=new int;
    *n=100;
    *n1=1;

    cout <<*n<<" "<<*n1<<endl;

    //申请一块连续堆空间
    char *p=new char[100];

    strcpy(p, "陈善杰");   //字符串拷贝到堆数组
    strcpy(p+10, "王乾宇"); //p+10：向后偏移，写入第二个字符串
    
    cout << p << endl;
    cout << p+10 << endl;

    //释放连续的堆空间
    delete [] p;

    //释放c
    free(n);
    //c++
    delete n1;
    
    return 0;
}