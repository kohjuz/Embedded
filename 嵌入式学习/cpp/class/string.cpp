#include <iostream>

using namespace std;

int main()
{
    string str1="唐蛋王乾宇";
    //追加字符串
    str1.append("是施炜杰儿子");
    cout<<str1<<endl;
    //赋值字符串
    //str1="陈善杰陈蛋";//赋值会覆盖原字符串(清空后赋值)
    str1.assign("陈善杰陈蛋",9,6);//一个中文占3个字节
    cout<<str1<<endl;
    //clear()清空字符串
    //compare()比较字符串是否相等
    //erase()删除字符串
    //find()查找字符串
    //substr()截取字符串
    //size()获取字符串长度
    
    string str2;
    cout<<"请输入字符串："<<endl;
    cin>>str2;
    cout<<"您输入的字符串是："<<str2<<endl;
    // for(int i=0;i<str2.size();i++)
    // {
    //     //如果两个字符相等就删除一个
    //     if(str2[i]==str2[i+1])
    //     {
    //         str2.erase(i,1);
    //     }
    // }
    //    cout<<str2<<endl;


    //面向对象编程
    string str3;
    for(int i=0;i<str2.size();i++)
    {

        if(str2[i]==str2[i+1])
        {
            continue;
        }
            str3+=str2[i];
        

    }
        cout<<str3<<endl;

    return 0;
}