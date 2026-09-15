#include <iostream>

using namespace std;


float vol(float r,float h=1,float pi=3.141592654)
{

    return (pi*r*r*h)/3;
}

int main()
{
    cout << "input r" << endl;

    float r;
    cin>>r;

    cout << vol(r) << endl;    




    return 0;
}
