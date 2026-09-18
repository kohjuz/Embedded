#include <iostream>

using namespace std;

class Cat
{

public:
    int x=0;
    Cat(int _x):x(_x){};
    Cat(){};
    Cat operator+(const Cat &p)
    {
        Cat add(this->x + p.x);
        return add;
    }
    Cat operator+(const int _x)
    {       
        Cat add(this->x + _x);
        return add;

    }
    friend Cat operator+(int num,const Cat &p);
    //友元重载<<用于cout打印
    friend ostream& operator<<(ostream &os, const Cat &obj);
};
ostream& operator<<(ostream &os, const Cat &obj)
{
    os<<obj.x;
    return os;
}
Cat operator+(int num,const Cat &p)
{
    return Cat(num+ p.x);
}

int main()
{
    Cat c1(5);
    Cat c2(8);
    Cat c3;
    c3=c1+c2;
    cout<<c3<<endl;
    c3=c1.operator+(c2);
    cout<<c3<<endl;
    c3=c1+2;
    cout<<c3<<endl;
    c3=6+c2+c1+c2;
    cout<<c3<<endl;

    return 0;
}