#include <iostream>
#include <string>
#include <string.h>

using namespace std;

string s1;
string s2;




int main()
{
    cout << "input:" << endl;
    cin >> s1;
    for(int i=0;i<s1.length();i++)
    {
        if(('a'<=s1[i])&&(s1[i]<='z'))
        {

            s2+='A'+(s1[i]-'a');
        }
        else if(('A'<=s1[i])&&(s1[i]<='Z'))
        {
            s2+='a'+(s1[i]-'A');

        }
        else
        {
            s2+=s1[i];
        }

    }

    cout << s2 << endl;



    return 0;
}
