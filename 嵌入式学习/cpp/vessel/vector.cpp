#include <iostream>
#include <vector>
//#include <algorithm>
#include <algorithm>

using namespace std;

vector<int>even_v1;
vector<int>odd_v1;

class even
{
public:
    bool operator()(int a,int b)
    {
        return a <b;
    }
};
class odd
{
public:
    bool operator()(int a,int b)
    {
        return a>b;
    }
};


int main()
{
    int n;
    cout<<"请任意输入n个数"<<endl;
    cin>>n;

    for(int i=0;i<n;i++)
    {
        int num;
        cin>>num;
        if(num%2==0)
        {
            even_v1.push_back(num);
        }
        else
            odd_v1.push_back(num);
    }
    sort(even_v1.begin(),even_v1.end(),even());//对容器里一段区间的元素进行排序（默认升序）
    sort(odd_v1.begin(),odd_v1.end(),odd());

    cout<<"结果";
    for(auto val:even_v1)
    {
        cout<<val<<" ";
    }
    for(auto val:odd_v1)
    {
        cout<<val<<" ";
    }
    cout<<endl;

    // vector<int>::iterator it =v1.begin();



    return 0;
}