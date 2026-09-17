#include <iostream>

using namespace std;

class dadcsj
{
public:
    dadcsj(string _name):name(_name)
    {
        
        cout<<name<<endl;
    }
protected:
string name;

};
class dadSWJ
{
public:
    dadSWJ()
    {
        cout<<"you are my son\n"<<endl;
    }

};
class Wqy:public dadcsj,public dadSWJ
{
public: 
    Wqy(string _name):dadcsj(_name),dadSWJ()
    {

    };

};

int main()
{
    Wqy tangdan("son");
    


}