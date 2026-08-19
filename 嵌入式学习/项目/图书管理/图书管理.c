#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "book_manager.h"



// 图书管理系统

//首先需要登录选项，分为用户登录和管理员登录，退出系统
//----------------------
//用户登录逻辑
//if(用户没注册){注册用户}--->回到用户登录界面
//else if(用户已注册){登录}--->进入功能选择
/*功能选择{
    图书查询
    {
        书籍编号
        书籍年份
        书籍作者
        书籍名字
        书籍类别
    }
    图书借阅
    {
        书籍编号||书籍名字||书籍类别||
        选择借阅的书籍
        if(借阅成功){存储借阅的信息}
        （
        借阅信息包括：
        用户名
        书籍编号
        书籍名字
        借阅时间
        借阅状态
        ）
        else{借阅失败}-->回到图书借阅}
        借阅成功后，判断是否继续借阅其他书籍
        if(继续借阅){继续借阅其他书籍}--->回到图书借阅界面}
        else{返回到功能选择}
    }
    图书归还
    {
        书籍编号||书籍名字||书籍类别||
        选择归还的书籍
        if(归还成功){存储归还的信息}
        （
        归还信息包括：
        用户名
        书籍编号
        书籍名字
        归还时间
        归还状态
        ）
        else{归还失败}-->回到图书归还}
        归还成功后，判断是否继续归还其他书籍
        if(继续归还){继续归还其他书籍}--->回到图书归还界面}
        else{返回到功能选择}
    }

    借阅查询
    {
        显示该用户的借阅书籍
        {显示借阅信息}
        返回到功能选择
    }
    退出---->退出系统
}
    退出系统---》程序结束

管理员登陆逻辑
{
    功能选择
    {
        图书信息更新
        {
            书籍编号，书籍名字，书籍类别，书籍年份，书籍作者
            选择要更新的书籍
            更新书籍信息
            返回到功能选择
        }
        图书信息录入
        {
            书籍编号
            书籍年份
            书籍作者
            书籍名字
            书籍类别
            录入书籍信息
            返回到功能选择
        }
        图书信息删除
        {
            书籍编号
            选择要删除的书籍
            删除书籍信息
            返回到功能选择
        }

        图书信息显示
        {
            书籍编号
            选择要显示的书籍
            更新书籍信息
            返回到功能选择
        }
        退出
        {
            退出系统
        }
       
    }

}


*/
//创建读者链表（也就是用户链表）  需要两个节点，一个是用户节点，一个是书籍节点
//用户节点包括用户名，借阅书籍的链表头指针，下一个用户的指针
//书籍节点包括书籍编号，书籍名字，书籍类别，书籍年份，书籍作者，借阅状态，借阅时间，归还时间







int choice;
int main(int argc, char *argv[])
{
    //初始化用户链表
    user_list head =list_user_init();
    //初始化书籍链表
    book_list book_head =list_book_init();
    if(head == NULL || book_head == NULL)
    {
        printf("head或book_head内存分配失败\n");
        return 0;
    }

    //从文件加载已注册用户
    load_users(head);

    //从文件加载借阅记录
    load_borrows(head);

    //从文件加载书籍
    load_books(book_head);
    //生成初始书籍状态文件
    save_book_status(book_head);

    while(1)
    {
    printf("--欢迎使用图书馆管理系统界面--\n");
    printf("1.用户登录\n");
    printf("2.管理员登录\n");
    printf("3.退出系统\n");
    int ret =scanf("%d",&choice);
    if(ret!=1)
    {
        printf("输入错误，请重新输入\n");
        while(getchar()!='\n');
        continue;
    }
    switch(choice)
    {
        case 1:
            user_d(head, book_head); //用户登录
            break;
        case 2:
            admin_d(head, book_head); //管理员登录
            break;
        case 3:
            printf("谢谢使用，再见！");
            return 0;
        default:
            printf("--输入错误，请重新输入--\n");
            break;
    }
    }






    return 0;
}