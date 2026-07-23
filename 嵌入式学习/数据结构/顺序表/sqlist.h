#ifndef __SQLIST_H
#define __SQLIST_H
#include <stdbool.h>
#include<stdlib.h>

typedef struct sqlist
{
    int *data;
    int cap;
    int last;
    
}sqlist;


sqlist *sqlist_init(int cap);
bool add_data(sqlist * sql,int n);
bool del_data(sqlist * sql,int n);

void output(sqlist *sql);
void reverse_output(sqlist *sql);

void restroy(sqlist *sql);

void sort(sqlist *sql);
#endif