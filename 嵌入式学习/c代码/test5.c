#include<stdio.h>

int main(int argc,char **argv)
{
    int len,count=0;
    int arr[len];
    printf("输入数组长度和移动次数（输入格式:a,b）");
    scanf("%d,%d",&len,&count);
    for(int i=0;i<len;i++)
    {
        arr[i]=i+1;

    }
       
        int n=len-count;
        
        while(n<len)
        {
            
            printf("%d ",*(arr+n));
            ++n;
        }
        
        for(int j=0;j<(len-count);j++)
        {
            printf("%d ",arr[j]);
        }




}