#include <iostream>
using namespace std;

class Cat
{
public:

    int age;
    //成员函数：比较当前对象(this) 和另外一只猫other
    Cat& getOlderCat(Cat &other)
    {
        // this->age 就是当前猫对象的年龄
        if(this->age >= other.age)
        {
            return *this; // *this：返回当前对象本身
        }
        else
        {
            return other;
        }
    }

};

int main()
{
    Cat cat1;
    cat1.age = 3;

    Cat cat2;
    cat2.age = 5;

    // cat1调用函数，this指向cat1，other是cat2
    Cat &oldCat = cat1.getOlderCat(cat2);
    cout << "年龄较大的猫：" << oldCat.age << endl;

    return 0;
}
