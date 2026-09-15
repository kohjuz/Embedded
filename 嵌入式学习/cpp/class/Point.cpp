#include <iostream>
#include <cmath>

using namespace std;

class Point
{
    public:
    Point(float _x,float _y)
    {
        x=_x;
        y=_y;
    }
    Point(float _x,float _y,string _name)
    {
        x=_x;
        y=_y;
        name=_name;
    }
    /*
    拷贝构造 //只有（初始化赋值）才会调用拷贝构造
    初始化赋值就是：int a=100 int b=a;
    常规赋值就是： int a=100; int b; b=a;
    只有一种写法， 不能重载
    不写拷贝构造函数，编译器会自动生成拷贝构造函数（把other所有成员变量赋值给当前对象）
    系统自己写的拷贝构造
    类名（const 类名 &other）
    {
        this->name=other.name;
        this->y=other.y;
        this->x=other.x;
        
    }
    类名（const 类名 &other）
    {
        
    }
    类名b=a;
    类名b(a);
    类名 *b=new 类名(a);
    */


    Point(const Point &other)
    {
        this->name=other.name;
        this->y=other.y;
        this->x=other.x;
    }

        /*
    移动构造：把右值的内存，指针直接转移给新对象
    类名（类名 &&other）
    {

    }
    情况1、返回一个临时对象
    情况2、用move调用
    */


    
    //情况2
    Point(Point &&other)
    {
        this->name=other.name;
        this->y=other.y;
        this->x=other.x;

        other.name.clear();
        other.y=0.0;
        other.x=0.0;
    }

    //防止隐式转换：explicit Point
    /*无参构造
    Point=default;
    */

    float fun(Point & other)
    {
        return sqrt(pow((this->x-other.x),2)+pow((this->y-other.y),2));
    }
    /*
    析构函数
    ~类名（）
    {
    }
    无重载，没有形参
    不写析构，编译器会自动生成析构
    */
    ~Point()
    {
        cout<<"析构函数释放空间"<<this->name<<endl;
    }

    private:

    float x;
    float y;
    string name;


};

int main()
{
    //若创建类对象为堆空间，要delete 释放
    //Point *p1=new Point(20,30);
    //delete p1;

    //这个是栈空间
    Point name1(50,20,"name1");
    Point name2(20,30,"name2");  
    Point name3=name2;  
    Point *name4= new Point(name3);
    Point name5=Point(name1);

    cout<<"两点之间距离"<<name1.fun(name2)<<endl;

    cout<<"两点之间距离"<<name1.fun(name3)<<endl;
    cout<<"两点之间距离"<<name1.fun(name1)<<endl;
    cout<<"两点之间距离"<<name1.fun(*name4)<<endl;

    cout<<"name5和name4两点之间距离"<<name5.fun(*name4)<<endl;
    cout<<"name1和name5两点之间距离"<<name1.fun(name5)<<endl;


    Point name6=move(name2);
    cout<<"name6和name1两点之间距离"<<name1.fun(name6)<<endl;
    cout<<"两点之间距离name2已经被转移为name6，相当于name1和(0,0)距离"<<name1.fun(name2)<<endl;
    //delete 显示禁用/删除该函数
    //拷贝构造，赋值，普通函数可以用delete
    //在声明的时候=delete 如void fun()=delete 禁用fun
    //类只创建一次，禁止赋值的类就要禁用拷贝构造

    //这个delete是释放堆空间
    delete name4;//释放堆

    return 0;

}