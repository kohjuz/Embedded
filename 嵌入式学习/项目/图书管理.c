#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "kernel_list.h"

// 图书管理系统

//创建读者链表
typedef struct read_user
{
    char user_name[50];
    struct read_user *next;
    struct read_user *book;/*book->next=head；head->next=book所以是个
    二级链表*/
    


}read_user,*user_list;
int main(int argc, char *argv[])
{

}