#include <iostream>

using namespace std;

class Ret
{
    public:

    Ret(string _name):name(_name)
    {
        
        cout<<"my is "<<this->name<<"构造"<<endl;
    }
    void set_h_w(float _h,float _w)
    {
        h=_h;
        w=_w;
        cout<<"长为："<<h<<endl;
        cout<<"宽为："<<w<<endl;
    }
    float s()
    {
        float num=h*w;
        
        return num;
    }
    ~Ret()
    {
        cout<<"析构函数释放空间"<<this->name<<endl;
    };


    protected:   //这个子类可以继承

    float w;
    float h;
    string name;
};
//子类大小=共享父类的数据成员变量和+子类变量
class zfx:public Ret
{
    public:
    zfx(string _name):Ret(_name)
    {
        
    }
    void f(float s)
    {
        w=s;
        h=s;
        cout<<"长为："<<h<<endl;
        cout<<"宽为："<<w<<endl;
    }

};

int main()
{
    Ret a1("dad");
    a1.set_h_w(2,3);
    cout<<"矩形面积为："<<a1.s()<<endl;

    zfx a2("son");
    a2.f(5);
    cout<<"正方形面积为："<<a2.s()<<endl;
    

    return 0;
}