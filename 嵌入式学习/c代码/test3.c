#include<stdio.h>
#include<string.h>
void f3(char **z,int len)
{
    for(int i=0;i<len;i++)
    {
          printf("%s",z[i]);

    }
  
}
int main()
{
    char *s[3]={"abc","123","xyz"};
    f3(s,3);



  
    return 0;
}







