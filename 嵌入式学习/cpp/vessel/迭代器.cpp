#include <iostream>
#include <vector>

//Frist: read and write 迭代器 begin end
//vector<int>::iterator it=v1.begin();
//Secon: only read 迭代器 cbeing and cend
//vector<int>::const_iterator it=v1.cbegin();
//Thirdly:contrary 迭代器 rbegin rend
//vector<int>::reverse_iterator it=v1.rbegin();
//Fourthly: const reverse 迭代器 crbegin crend
//vector<int>::const_reverse_iterator it=v1.crbegin();

//v1.assign(v2.begin(),v2.begin+n) v2前n-1个数据赋值给v1
//vector转化为c语言的int * 指向起始元素地址 int *p=v1.data();

//判断容器是否为空 v1.empty();
//erase :  v1.erase();    erase(first,last)  相当于[first,last)  

//v1.insert(a,b)  把b插入到a前面
//v1.insert(v1.begin(),区间范围)区间范围：v2.begin(),v2.end(),相当于把v2插到V1前面

using namespace std;

vector<int>v1;


int main()
{
    v1.push_back(20);
    v1.push_back(30);
    v1.push_back(40);
    v1.push_back(50);
    cout << "1.读写迭代器 iterator：";
    //read and wirte
    vector<int>::iterator it=v1.begin();
    //或者写auto it = v1.begin();
    for(;it!=v1.end();it++)
    {
        cout<<*it<<"";
        *it+=10;
        cout<<"+10后："<<*it<<"    ";
    }
    cout << endl;
    //only read
    cout << "2.只读迭代器 const_iterator：";
    vector<int>::const_iterator cit=v1.cbegin();
    //或者写auto cit = v1.cbegin();
    for(;cit!=v1.end();cit++)
    {
        cout<<*cit<<" ";
    }
    cout << endl;
    //3.反向迭代器 reverse_iterator rbegin rend 逆序遍历
    cout << "3.反向迭代器 reverse_iterator：";
    vector<int>::reverse_iterator rit = v1.rbegin();
    //或者写auto rit = v1.rbegin();
    for(rit = v1.rbegin(); rit != v1.rend(); rit++)
    {
        *rit += 10; //反向迭代器 可读可写
        cout << *rit << " ";
    }
    cout << endl;

    //4.const反向只读迭代器 crbegin crend
    cout << "4.反向只读迭代器 const_reverse_iterator：";
    vector<int>::const_reverse_iterator crit = v1.crbegin();
    //或者写auto crit = v1.crbegin();
    for(crit = v1.crbegin(); crit != v1.crend(); crit++)
    {
        //*crit = 100;
        cout << *crit << " ";
    }
    cout << endl;

    return 0;
}