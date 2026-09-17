#include <iostream>

using namespace std;

class Animal
{
    int age;
    int weight;
    public:
    Animal(int a,int w):age(a),weight(w){};
    friend void change_animal(Animal &p,int _age,int _weight);
    void show()
    {
        cout<<age<<" "<<weight<<endl;
    }

};
void change_animal(Animal &p,int _age,int _weight)
{
    p.age=_age;
    p.weight=_weight;
}

int main()
{
    Animal a(29,30);
    a.show();
    change_animal(a,100,20);
    a.show();




    return 0;
}