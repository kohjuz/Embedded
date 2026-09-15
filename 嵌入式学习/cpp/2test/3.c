#include <iostream>
using namespace std;

string s1="csj";
string s2="wqy";


//交换s1和s2的内容
void strarr(string &s1, string &s2)
{
    string temp;
    temp=s1;
    s1=s2;
    s2=temp;

}

int main()
{
    cout<<s1<<endl;
    cout<<s2<<endl;
    strarr(s1,s2);
    cout<<s1<<endl;
    cout<<s2<<endl;


    return 0;
}
