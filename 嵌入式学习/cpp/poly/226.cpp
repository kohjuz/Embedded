#include <iostream>

using namespace std;

class B
{
public:
    virtual void vf()
    {
        cout<<"B"<<endl;

    }


};
class D:public B
{
public:
    void vf()
    {
        cout<<"D"<<endl;
    }

};

int main()
{
    B b,*pb;
    D d,*pd;
    pb=&b;pb->vf();
    pd=&d;pd->vf();
    pd=(D*)&b;pd->vf();
    pd=&d;pd->vf();



    return 0;
}