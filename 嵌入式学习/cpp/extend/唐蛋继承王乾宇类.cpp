#include <iostream>

using namespace std;

class Wqy
{
    public:
    void tang()
    {
        cout<<"唐"<<endl;
    }
    void tian()
    {
        cout<<"甜"<<endl;
    }



};
    class tangdan:public Wqy //唐蛋继承王乾宇，子类继承父类
    //唐蛋公有继承父类王乾宇
    {

    };

int main()
{
    tangdan wqy1;//唐蛋虽然没那么唐，但是继承了王乾宇的本领
    wqy1.tang();
    wqy1.tian();
    return 0;
}