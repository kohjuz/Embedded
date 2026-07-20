//    #include<stdio.h>
//    #include<string.h>
   
//    int main()
//    {
        
//         char str[100];
//         printf("???????");
//         scanf("%s",str);
//         int len = strlen(str);
  
//         for(int i=0;i<strlen(str);i++)
//         {

//             for(int j=i+1;j<strlen(str);j++)
//             {
//                 if(str[i]==str[j])
//                 {
//                     for(int k=j;k<len;k++)
//                     {
//                         str[k]=str[k+1];
                        
//                     }
                    
//                     len--;

//                 }
                
               
//             }
            
//         printf("%c",str[i]);
//         }



//     return 0;
//    }
 
#include<stdio.h>

void main()
{
    bool ch[255]={0};
    char a[100];
    printf("??????:");
    scanf("%s",&a);
    int len =srtlen(a);
    for(int i=0;i<len;i++)
    {
        if(ch[a[i]]==false)
        {
            
            ch[a[i]]=true;
            

        }
    }



}