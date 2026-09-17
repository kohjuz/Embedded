#include <iostream>
#define Pi 3.14
using namespace std;

class Base
{
    protected:
    int w=0;
    int h=0;
    int r=0;


public:
    Base(int _w,int _h):w(_w),h(_h)
    {
        cout <<"w: "<<w<<"h: "<<h <<endl;
    }
    Base(int _r):r(_r)
    {
        cout<<"r: "<<r <<endl;
    }
 virtual void getArea()=0;

};
class Rect:public Base
{

public:
    Rect(int _w,int _h):Base(_w,_h){}
    
    void getArea()
    {
        
        cout<<"矩形面积为："<<h*w<<endl;
    }

};
class Circle:public Base
{
public:
    Circle(int _r):Base(_r){}
    void getArea()
    {
        
        cout<<"圆形面积为："<<Pi*r*r<<endl;
    }
};

int main()
{
    Circle name(2);
    name.getArea();
    Rect name1(20,30);
    name1.getArea();


    return 0;
}