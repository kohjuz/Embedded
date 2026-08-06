#include<stdio.h>
void ab(int* a,int* b)
{
    int p;
     p= * a;
    * a= * b;
    * b= p;

    
}
void transform(int *b,int **p)
{
    *p=b;



}
void susu(int value);
void main()
{

     int a; 
     int b;
    int *p;
    // p=&a;
    // ab(&a,&b);
    // printf("%d\n",*p);
    // transform(&b,&p);
    // printf("%d\n",*p);
    // scanf("%d %d",&a,&b);
    // printf("%d,%d",a,b);
    // printf("%d\n",getbit(3,3));
    // printf("%d",getbit(3,1));
    // printf("%d,%d",a,b);
    susu(5);
    susu(16);
    susu(2);


}
void susu(int value)
{
    for(int i=1;i<value;i++)
    {
        if(value%i==0)
        {
            printf("n:%d\n",value);
            

        }
       if((i==value-1)&&(value%1==0)&&(value%value==0))
       {
        printf("y:%d\n",value);
       }


    }
    
}

int getbit(int value,int n)
{
    int n1;
    n1=(value>>n-1)&&0x01;
    return n1;
}