#include <iostream>
#include <cmath>
using namespace std;

// 基类 Shape
class Shape
{
public:
    // 虚函数，周长
    virtual double getPerimeter() const = 0;
    // 虚函数，面积
    virtual double getArea() const = 0;
    virtual ~Shape(){} // 虚析构
};

// 圆形类，继承Shape
class Circle : public Shape
{
private:
    double r; //半径
public:
    Circle(double _r):r(_r){}
    double getPerimeter() const override
    {
        return 2 * M_PI * r;
    }
    double getArea() const override
    {
        return M_PI * r * r;
    }
};

// 矩形类
class Rect : public Shape
{
private:
    double w, h; //宽、高
public:
    Rect(double _w,double _h):w(_w),h(_h){}
    double getPerimeter() const override
    {
        return 2*(w + h);
    }
    double getArea() const override
    {
        return w * h;
    }
};

// 三角形（这里用三边 a,b,c）
class Triangle : public Shape
{
private:
    double a,b,c; //三条边长
public:
    Triangle(double _a,double _b,double _c):a(_a),b(_b),c(_c){}
    double getPerimeter() const override
    {
        return a+b+c;
    }
    //海伦公式求面积
    double getArea() const override
    {
        double p = (a+b+c)/2.0;
        return sqrt(p*(p-a)*(p-b)*(p-c));
    }
};

int main()
{
    //多态测试
    Shape *p1 = new Circle(2);
    cout << "圆形：周长=" << p1->getPerimeter() << "，面积=" << p1->getArea() << endl;

    Shape *p2 = new Rect(3,4);
    cout << "矩形：周长=" << p2->getPerimeter() << "，面积=" << p2->getArea() << endl;

    Shape *p3 = new Triangle(3,4,5);
    cout << "三角形：周长=" << p3->getPerimeter() << "，面积=" << p3->getArea() << endl;

    delete p1;
    delete p2;
    delete p3;
    return 0;
}
