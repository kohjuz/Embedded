//template<typename T>
//template<class T>
#include <iostream>
#include <cmath>

using namespace std;

template<typename T>
T fun(T num)
{
    return pow(num,2);
}



int main()
{
    cout<<fun(20)<<endl;
    
    cout<<fun(2.33)<<endl;
    cout<<fun(0.1)<<endl;
    

    return 0;
}