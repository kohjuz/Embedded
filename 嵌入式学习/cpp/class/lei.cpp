#include <iostream>

using namespace std;

//矩形类
class jx
{
    public:


    void s()
    {
        cout <<"s"<< a*b<<endl;
    }
    void c()
    {
        cout <<"c: "<<2*(a+b)<<endl;
    }
    void set(float w,float h)
    {
        a=w;
        b=h;
    }
    private:
    float a;
    float b;

};

//用struct 定义类默认全部public
// struct Rect
// {

// };
int main()
{
    
    jx ab;
    ab.set(20,23);
    ab.s();
    ab.c();
    jx *wqy=new jx;
    wqy->set(30,20);
    wqy->s();
    wqy->c();

    delete wqy;

    return 0;
}