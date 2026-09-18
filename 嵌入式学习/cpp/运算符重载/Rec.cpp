#include <iostream>

using namespace std;

class Rect
{

public:
    int x=0,y=0;
Rect(int _x, int _y):x(_x),y(_y){};
    // 单int构造：用于 int转Rect，宽高都等于这个数字
    Rect(int _num):x(_num),y(_num){};
    Rect(){};
    Rect operator+(const Rect &p)
    {
        Rect add(this->x + p.x,this->y + p.y);
        return add;
    }
    Rect operator+(const int _num)
    {       
        Rect add(this->x + _num,this->y + _num);
        return add;

    }
    Rect operator-(const Rect &p)
    {
        Rect add(this->x - p.x,this->y - p.y);
        return add;
    }
    Rect operator-(const int _num)
    {       
        Rect add(this->x - _num,this->y -_num);
        return add;

    }
    friend Rect operator+(int num,const Rect &p);
    friend Rect operator-(int num,const Rect &p);
    //友元重载<<用于cout打印
    friend ostream& operator<<(ostream &os, const Rect &obj);
};
ostream& operator<<(ostream &os, const Rect &obj)
{
    os<<obj.x;
    return os;
}
Rect operator+(int num,const Rect &p)
{
    return Rect(num+ p.x,num+ p.y);
}
Rect operator-(int num,const Rect &p)
{
    return Rect(num- p.x,num- p.y);
}


int main()
{
    Rect r1(2,3);
    Rect r2(4,5);
    Rect r3(1,1);
    Rect r4;
    r4=9+r1+5+r2+6+r3-7;
    cout<<r4<<endl;

    return 0;
}