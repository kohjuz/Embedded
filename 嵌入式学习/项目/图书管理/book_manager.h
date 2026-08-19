#ifndef BOOK_MANAGER_H
#define BOOK_MANAGER_H

#include "kernel_list.h"

typedef struct read_user
{
    char user_name[50];//用户名
    char password[50];//密码

    struct read_user *next;//下一个用户的指针
    struct root_book *book;//借阅书籍的链表头指针
}read_user,*user_list;

//创建书籍链表
//书籍节点包括书籍编号，书籍名字，书籍类别，书籍年份，书籍作者，借阅状态，借阅时间，归还时间
//书籍节点的指针为next
typedef struct root_book
{
    char book_id[50];//书籍编号
    char book_name[50];//书籍名字
    char book_type[50];//书籍类别
    int book_year;//书籍年份
    char book_author[50];//书籍作者
    char book_status[50];//借阅状态
    char book_borrow_time[50];//借阅时间
    char book_return_time[50];//归还时间
    struct root_book *next;//下一个书籍的指针

}book,*book_list;

user_list list_user_init();//初始化用户链表
book_list list_book_init();//初始化书籍链表
void load_books(book_list book_head);//加载书籍信息
void save_book_status(book_list book_head);//保存书籍状态
read_user *create_user(char *name, char *pwd);//创建用户节点
void load_users(user_list head);//加载用户信息
void save_user_to_file(char *name, char *pwd);//保存用户信息
void save_borrow_to_file(char *username, book_list book);//保存借阅信息
void remove_borrow_from_file(char *username, char *book_id);//删除借阅信息
void load_borrows(user_list head);//加载借阅信息
void user_register(user_list head, book_list book_head);//用户注册
read_user *user_login(user_list head);//用户登录
void borrow_book(read_user *user, book_list book_head);//用户借阅书籍
void return_book(read_user *user, book_list book_head);//用户归还书籍
void display_book_info(read_user *user);//显示用户借阅书籍信息
void user_gn_menu(read_user *user, book_list book_head);//用户菜单操作
void user_d(user_list head, book_list book_head);//用户入口
int admin_login();//管理员登录
void admin_add_book(book_list book_head);//管理员添加书籍
void admin_delete_book(book_list book_head);//管理员删除书籍
void admin_update_book(book_list book_head);//管理员更新书籍
void admin_show_all_books(book_list book_head);//管理员显示所有书籍
void admin_gn_menu(user_list head, book_list book_head);//管理员菜单操作
void admin_d(user_list head, book_list book_head);//管理员入口



#endif
