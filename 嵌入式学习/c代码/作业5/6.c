#include<stdio.h>
#include<string.h>
char str(char a[])
{
    char b[100];
    int ide=0;
    for(int i=0;i<strlen(a);i++)
    {
        
        if(a[i]==' ')
        continue;
        b[ide++]=a[i];
    }
    b[ide]='\0';
    
        printf("%s",b);
    

}
int main()
{
    char *a="a b b c";
    str(a);
  



    return 0;
}