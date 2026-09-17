#include <iostream>

using namespace std;

class student
{
    public:
    student(string _name,int g)
    {
        sum+=g;
        str += to_string(g);//将分数强转为字符串
        name+=_name;

        cout<<"现在有"<<n<<"个学生\n"<<name<<"分数为"<<str<< "当前平均分："<<sum/n<<endl;
        n+=1;
        str=str+'+';
        name=name+'+';

    }


    private:
    static string name;
    static string str;
    static int sum;
    static int n;

};
string student::name;
string student::str;
int student::sum=0;
int student::n=1;

int main()
{
    student wqy("王乾宇",20);
    student csj("陈善杰",30);
    student tang("良子",25);
    student a("唐蛋",25);
    return 0;
}