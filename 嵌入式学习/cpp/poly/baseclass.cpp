#include <iostream>

using namespace std;

class BassClass
{
public:
    virtual ~BassClass()
    {
        cout <<"~BassClass()"<<endl;
    }

};
class DerivedClass:public BassClass
{
public:
    ~DerivedClass()
    {
        cout <<"DerivedClass()"<<endl;
    }
};

int main()
{
    BassClass *p=new DerivedClass;

    delete p;
    return 0;
}