#include <iostream>

using namespace std;

class Rec
{
    int w=0;
    int h=0;
    string c;
    public:


    friend class Ui;
    void show()
    {
        cout<<w<<" "<<h<<" "<<c<<endl;
    }
};

class Ui
{
    public:
    void set(Rec &other)
    {
        other.c="aaa";
        other.w=2;
        other.h=3;
    }

};

int main()
{
    Rec a;
    Ui b;
    a.show();
    b.set(a);
    a.show();

    return 0;
}
