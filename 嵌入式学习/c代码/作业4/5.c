#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void rand_number(int m,int n,int a[m][n])
{
    for(int i=0;i<m;i++)
    {
        for(int k=0;k<n;k++)
        {
            a[i][k]=rand()%1000;
        }
    }
}
void show(int m,int n,int a[m][n])
{
        for(int i=0;i<m;i++)
    {
        for(int j=0;j<n;j++)
        {
            printf("%d\t",a[i][j]);
        }
        printf("\n");

    }

}
void cpstr(int m,int n,int a[m][n],int b[m][n])
{
 
    memcpy(b,a,m*n*sizeof(int));

}
int main()
{
    int m,n;
    scanf("%d,%d",&m,&n);
    int a[m][n];
    int b[m][n];
    rand_number(m,n,a);
    show(m,n,a);
    cpstr(m,n,a,b);
    show(m,n,b);



    return 0;
}