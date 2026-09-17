#include <iostream>

using namespace std;

class Animal
{
public:
    virtual void eat() 
    {
        cout<<"Animal eat"<<endl;
    }

};

class Sheep:public Animal
{
public:
    void eat() override
    {
        cout<<"Sheep eat"<<endl;
    }
};
class leng:public Animal
{
public:
    void eat() override
    {
        cout<<"leng eat"<<endl;
    }

};
void eatmin(Animal * other)
{
    other->eat();
}


int main()
{
    Animal a1;
    leng b2;
    Sheep c3;

    eatmin(&a1);
    eatmin(&b2);
    eatmin(&c3);



    return 0;
}