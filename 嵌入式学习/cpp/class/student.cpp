#include <iostream>
#include <string>
using namespace std;

class Ain
{
    public:
    typedef struct student
    {
        int age;
        string name;
        student(string n,int a):name(n),age(a){}
    }stu;

    stu s;
    Ain(string name,int _age):s(name,_age)
    {
        cout<<s.name<<" "<<s.age<<endl;
    };
    Ain():s(" ",0)
    {};
    Ain operator+(const Ain &p)
    {
        int new_age=this->s.age +p.s.age;
        string new_name=this->s.name +"+"+p.s.name;
        return Ain(new_name,new_age);
    }
    Ain operator++(int)
    {
        Ain temp = *this; // 保存对象全部数据name+age
        this->s.age++;
        return temp;
    }

    Ain operator++()
    {
        
        this->s.age=this->s.age+1;
        return *this;
    }

    friend istream& operator>>(istream &is,const Ain &obj);


};
ostream& operator<<(ostream &os, const Ain &obj)
{
    os << "name:" << obj.s.name << ", age:" << obj.s.age;
    return os;
}
istream& operator>>(istream &is,Ain &obj)
{
    is>>obj.s.name;
    is>>obj.s.age;
    return is;
}


int main()
{
    Ain w("王乾宇",21);
    Ain csj("陈善杰",22);
    Ain sum;

    sum=w+csj;
    cout<<sum<<endl;
    w++;
    cout<<w<<endl;
    ++w;
    cout<<w<<endl;
    return 0;
}