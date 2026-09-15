#include <iostream>

using namespace std;

class Kitty
{
    private:

    float weight;
    string sex;//有male和female两种
    string color;//有black、white、brown三种
    

    public:

    Kitty()
    {
        weight=0;
        sex="unknown";
        color="unknown";
    }
    ~Kitty()
    {
        cout<<"Kitty对象被销毁"<<endl;
    }


    void eat(){cout<<"吃鱼"<<endl;};
    void sleep(){cout<<"睡觉"<<endl;};
    void actingCute(){cout<<"卖萌"<<endl;};
    void info()
    {
        cout<<"体重:"<<weight<<endl;
        cout<<"性别:"<<sex<<endl;
        cout<<"颜色:"<<color<<endl;
    }

    void setWeight(float w){weight=w;};
    void setsex(string s){sex=s;};
    void setcolor(string c){color=c;};



};

int main()
{
    cout<<"创建小猫"<<endl;
    Kitty cat;
    cat.setWeight(5.5);
    cat.setsex("female");
    cat.setcolor("white");

    cat.info();

    cat.eat();
    cat.sleep();
    cat.actingCute();



    return 0;

}