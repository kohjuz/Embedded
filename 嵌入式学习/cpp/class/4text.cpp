#include <iostream>
using namespace std;

int main()
{
    string str1;
    cout<<"请输网址："<<endl;
    cin>>str1;
    if((str1.find("http://www.")==0)||(str1.find("https://www.")==0)||(str1.find("www.")==0))
    {
        if(str1.find(".com")!= string::npos)
        {
        cout<<"这是一个网址"<<endl;
        cout<<"您输入的网址是："<<str1<<endl;
        }
        else
        {
            cout<<"这不是一个网址"<<endl;
        }
    }
    else
    {
        cout<<"这不是一个网址"<<endl;
    }


    return 0;
}
