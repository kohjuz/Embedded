#include<stdio.h>
#include<string.h>
#include<stdint.h>
char str(char a[],char b[])
{
    int len=strlen(a)+strlen(b)+1;
    char c[len];
    int count=0;
    for(int i=0;i<len;i++)
    {   
        if(a[i]=='\0')
        {
            count=i;
            continue;
        }
        if(count=i)
        {
                for(int j=count;j<len;j++)
                {
                    c[j]=b[i-j];

                }

        }
        c[i]=a[i];
        
    }
    
    for(int k=0;k<len;k++)
    {
        printf("%c",c[k]);
    }

    
}
int main()
{
    char *a="abcd";
    char *b="efgh";
    str(a,b);
    



    return 0;
}