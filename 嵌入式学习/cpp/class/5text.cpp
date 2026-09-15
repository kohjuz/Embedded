#include <iostream>
using namespace std;

int main()
{
    string A="hkdaskhqohhhaah";
    string B;
    cout<<"A字符串为："<<A<<endl;
    cout<<"请输入B字符串:"<<endl;

    cin>>B;

    string res;
    int i=0;
    while(i<A.size())
    {
        
        // 如果B中找不到A[i]这个字符，就保留到res
        if(B.find(A[i]) == string::npos)
        {
            res += A[i];
        }
    i++;
    }
    cout <<"删除B中所有字符后，A："<<res<<endl;
}