#include <iostream>
#include <string>
using namespace std;


namespace swj
{
    namespace son
    {
        string name="王乾宇";
        string name1="陈善杰";
        
    }
    void print()
    {
        cout <<"my son 2"<<swj::son::name<< endl;

    }
}
using swj::son::name1;

int main() 
{   
    cout << "my son 1"<<name1<<endl;
    swj::print();
    return 0;
}