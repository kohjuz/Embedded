#include <iostream>
#include <cmath>
#include <string>

using namespace std;

//圆形面积
double area(double r)
{
    return 3.14*r*r;
}
//三角形面积
double area(double a,double b,double c)
{
    double p=(a+b+c)/2;
    return sqrt(p*(p-a)*(p-b)*(p-c));//海伦公式
}
//矩形面积
double area(double a,double b)
{
    return a*b;
}

int main() 
{
    cout <<"圆形面积：" << area(3.0) << endl;
    cout <<"三角形面积：" << area(3.0,2.1,5.0) << endl;
    cout <<"矩形面积：" << area(3.0,4.0) << endl;

    return 0;
}