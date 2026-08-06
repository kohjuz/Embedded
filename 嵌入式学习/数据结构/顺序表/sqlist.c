#include <stdio.h>
#include "sqlist.h"



sqlist *sqlist_init(int cap)
{
    sqlist * sql=malloc(sizeof(sqlist));
    if(sql!=NULL)
    {
        sql->data=calloc(cap,sizeof(int));
        if(sql->data==NULL)
        {
            free(sql);
            return NULL;
        }
    }
    sql->last=-1;
    sql->cap=cap;

    return sql;

}
bool add_data(sqlist * sql,int n)
{
    if(sql->last>=sql->cap-1)
    {
        return false;
    }
    else
    {
        sql->data[++sql->last]=n;
        return true;
    }
    
}
bool is_empty(sqlist *sql)
{
    return sql->last ==-1;
}

bool del_data(sqlist * sql,int n)
{
    if(is_empty(sql))
    {
        return false;
    }
    else
    {
        for(int i=0;i<=sql->last;i++)
        {
            if(sql->data[i] == n)
            {
                for(int k=i;k<sql->last;k++)
                {
                    sql->data[k]=sql->data[k+1];
                    sql->last--;

                    return true;
                }
            }
        }
    }

}

void output(sqlist *sql)
{
    if(is_empty(sql))
    {
        return;
    }
    else
    {
        for(int i=0;i<=sql->last;i++)
        printf("%d\t",sql->data[i]);
        printf("\n");
    }
}

void reverse_output(sqlist *sql)
{
    if(is_empty(sql))
    {
        return;
    }
    else
    {
        for(int i=sql->last;i>=0;i--)
        printf("%d\t",sql->data[i]);
        printf("\n");
    }
}
void restroy(sqlist *sql)
{
    if(sql!=NULL)
    {
        free(sql->data);
    }
    free(sql);
    
}
void swap(int *p,int *q)
{
    int tmp;
    tmp = *p;
    *p = *q;
    *q = tmp;
}

void sort(sqlist *sql)
{
    if(is_empty(sql))
    {
        return;
    }
    int k=-1;
    int sorted =1;
    while(sql->last-1>k)
    {
        k++;
        
        if(sql->data[k]>sql->data[k+1])
        {
            sorted=0;
            break;
        
        }
    }
    if(sorted)
    {
         return;//返回
    }
    for(int i=0;i<sql->last;i++)
    {
        for(int j=0;j<sql->last-i;j++)//每次排完一次序,last向前一位
        {
            if(sql->data[j]>sql->data[j+1])
            {
                //交换
                swap(&sql->data[j],&sql->data[j+1]);
            }
        }
    }


}
    
  



    
