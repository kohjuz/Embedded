#include <iostream>

using namespace std;

class Cat
{

public:
    int x=0;
    string name;
    Cat(string _name,int _x):x(_x){};
    Cat(){};
    bool operator>(const Cat &p)
    {
        if(this->x > p.x)
        {
            return true;
        }
        else
            return false;

    }
    bool operator<(const Cat &p)
    {
        if(this->x < p.x)
        {
            return true;
        }
        else
            return false;

    }
    bool operator==(const Cat &p)
    {
        if(this->x == p.x)
        {
            return true;
        }
        else
            return false;

    }
    bool operator!=(const Cat &p)
    {
        if(this->x != p.x)
        {
            return true;
        }
        else
            return false;

    }

    
    


};


int main()
{
    Cat c1("小猫乾宇",5);
    Cat c2("小唐",7);
    Cat c3("小蛋",7);
    cout<<(c2>c1)<<endl;
    cout<<(c2==c3)<<endl;
    cout<<(c3<c1)<<endl;

    return 0;
}