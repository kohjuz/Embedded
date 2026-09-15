#include <iostream>
using namespace std;

struct Student
{
    string name;
    int age;
};

Student stu1 = {"王乾宇", 999};
int a=100;

int *p=&a;

float b=3.14;
//联合体
union Union
{
    int a;
    float b;
};
//枚举
enum Color
{
    Red,
    Green,
    Blue
};

Union u1={200};
int main() 
{
    cout << stu1.name <<" "<< p <<" " << u1.a <<" "<<Green << endl;
    return 0;
}