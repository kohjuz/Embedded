#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "book_manager.h"


user_list list_user_init()
{
    user_list head = (user_list)malloc(sizeof(read_user));
    if(head != NULL)
    {
        head->book = NULL;
        head->next = NULL;
    }
    return head;
}
book_list list_book_init()
{
    book_list head = (book_list)malloc(sizeof(book));
    if(head != NULL)
    {
        head->next = NULL;
    }
    return head;
}

//从文件加载书籍到链表
void load_books(book_list book_head)
{
    FILE *fp = fopen("books.txt", "r");
    if(fp == NULL)
    {
        printf("警告：未找到books.txt，书籍链表为空\n");
        return;
    }

    char line[256];
    //跳过第一行表头
    fgets(line, sizeof(line), fp);

    char id[50], name[50], author[50];
    int year;
    while(fscanf(fp, "%s\t%s\t%s\t%d", id, name, author, &year) == 4)
    {
        book_list node = (book_list)malloc(sizeof(book));
        if(node == NULL)
            break;

        strcpy(node->book_id, id);
        strcpy(node->book_name, name);
        strcpy(node->book_author, author);
        node->book_year = year;
        strcpy(node->book_type, "未分类");
        strcpy(node->book_status, "可借阅");
        strcpy(node->book_borrow_time, "");
        strcpy(node->book_return_time, "");
        node->next = NULL;

        //尾插到链表
        book_list tail = book_head;
        while(tail->next != NULL)
            tail = tail->next;
        tail->next = node;
    }
    fclose(fp);
}

//保存所有书籍状态到文件（每次借阅/归还时调用）
void save_book_status(book_list book_head)
{
    FILE *fp = fopen("book_status.txt", "w");
    if(fp == NULL)
        return;

    fprintf(fp, "书籍编号\t书籍名称\t作者\t年份\t状态\t借阅时间\n");
    book_list p = book_head->next;
    while(p != NULL)
    {
        fprintf(fp, "%s\t%s\t%s\t%d\t%s\t%s\n",
                p->book_id, p->book_name, p->book_author,
                p->book_year, p->book_status,
                strlen(p->book_borrow_time) > 0 ? p->book_borrow_time : "无");
        p = p->next;
    }
    fclose(fp);
}
read_user *create_user(char *name, char *pwd)
{
    read_user *user = (read_user *)malloc(sizeof(read_user));
    if(user != NULL)
    {
        strcpy(user->user_name, name);
        strcpy(user->password, pwd);
        user->next = NULL;
        user->book = NULL;
    }
    return user;
}

//从文件加载用户到链表
void load_users(user_list head)
{
    FILE *fp = fopen("users.txt", "r");
    if(fp == NULL)
        return; //文件不存在，第一次运行，无需加载

    char name[50];
    char pwd[50];
    while(fscanf(fp, "%s %s", name, pwd) == 2)
    {
        read_user *user = create_user(name, pwd);
        if(user == NULL)
            break;

        //尾插到链表
        read_user *tail = head;
        while(tail->next != NULL)
            tail = tail->next;
        tail->next = user;
    }
    fclose(fp);
}

//保存单个用户到文件（追加模式）
void save_user_to_file(char *name, char *pwd)
{
    FILE *fp = fopen("users.txt", "a");
    if(fp == NULL)
    {
        printf("警告：无法保存用户数据到文件\n");
        return;
    }
    fprintf(fp, "%s %s\n", name, pwd);
    fclose(fp);
}

//保存借阅记录到文件（追加）
void save_borrow_to_file(char *username, book_list book)
{
    FILE *fp = fopen("borrows.txt", "a");
    if(fp == NULL)
    {
        printf("警告：无法保存借阅记录\n");
        return;
    }
    //用|分隔，因为时间字符串里有空格
    fprintf(fp, "%s|%s|%s|%s|%d|%s\n",
            username, book->book_id, book->book_name,
            book->book_author, book->book_year, book->book_borrow_time);
    fclose(fp);
}

//从文件删除借阅记录（归还时调用）
void remove_borrow_from_file(char *username, char *book_id)
{
    FILE *fp = fopen("borrows.txt", "r");
    if(fp == NULL)
        return;

    FILE *tmp = fopen("borrows_tmp.txt", "w");
    if(tmp == NULL)
    {
        fclose(fp);
        return;
    }

    char line[512];
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        char file_user[50], file_id[50];
        //解析前两个字段
        sscanf(line, "%[^|]|%[^|]|", file_user, file_id);
        //不是要删除的记录就保留
        if(!(strcmp(file_user, username) == 0 &&
             strcmp(file_id, book_id) == 0))
            fputs(line, tmp);
    }
    fclose(fp);
    fclose(tmp);
    remove("borrows.txt");
    rename("borrows_tmp.txt", "borrows.txt");
}

//从文件加载所有借阅记录到对应用户
void load_borrows(user_list head)
{
    FILE *fp = fopen("borrows.txt", "r");
    if(fp == NULL)
        return;

    char line[512];
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';

        //按|分割: username|id|name|author|year|time
        char username[50], id[50], name[50], author[50], btime[50];
        int year;

        char *token = strtok(line, "|");
        if(token == NULL) continue;
        strcpy(username, token);

        token = strtok(NULL, "|");
        if(token == NULL) continue;
        strcpy(id, token);

        token = strtok(NULL, "|");
        if(token == NULL) continue;
        strcpy(name, token);

        token = strtok(NULL, "|");
        if(token == NULL) continue;
        strcpy(author, token);

        token = strtok(NULL, "|");
        if(token == NULL) continue;
        year = atoi(token);

        token = strtok(NULL, "|");
        if(token == NULL) continue;
        strcpy(btime, token);

        //找到对应用户，添加借阅记录
        read_user *p = head->next;
        while(p != NULL)
        {
            if(strcmp(p->user_name, username) == 0)
            {
                book_list node = (book_list)malloc(sizeof(book));
                strcpy(node->book_id, id);
                strcpy(node->book_name, name);
                strcpy(node->book_author, author);
                node->book_year = year;
                strcpy(node->book_type, "未分类");
                strcpy(node->book_status, "已借阅");
                strcpy(node->book_borrow_time, btime);
                strcpy(node->book_return_time, "");
                node->next = NULL;

                if(p->book == NULL)
                    p->book = node;
                else
                {
                    book_list tail = p->book;
                    while(tail->next != NULL)
                        tail = tail->next;
                    tail->next = node;
                }
                break;
            }
            p = p->next;
        }
    }
    fclose(fp);
}

//用户注册
void user_register(user_list head, book_list book_head)
{
    char input_name[50];
    char input_pwd[50];

    printf("=== 用户注册 ===\n");
    printf("请输入用户名：");
    scanf("%s", input_name);

    //检查用户名是否已存在
    read_user *p = head->next;
    while(p != NULL)
    {
        if(strcmp(p->user_name, input_name) == 0)
        {
            printf("用户名已存在，注册失败！\n");
            return;
        }
        p = p->next;
    }

    printf("请输入密码：");
    scanf("%s", input_pwd);

    //创建新用户并尾插到链表
    read_user *user = create_user(input_name, input_pwd);
    if(user == NULL)
    {
        printf("内存分配失败，注册失败！\n");
        return;
    }

    //找到链表末尾，插入新节点
    read_user *tail = head;
    while(tail->next != NULL)
        tail = tail->next;
    tail->next = user;

    //保存到文件
    save_user_to_file(input_name, input_pwd);

    printf("注册成功！\n");
}

//用户登录，返回用户节点指针，失败返回NULL
read_user *user_login(user_list head)
{
    char input_name[50];
    char input_pwd[50];

    printf("=== 用户登录 ===\n");
    printf("请输入用户名：");
    scanf("%s", input_name);
    printf("请输入密码：");
    scanf("%s", input_pwd);

    //遍历用户链表查找匹配
    read_user *p = head->next;
    while(p != NULL)
    {
        if(strcmp(p->user_name, input_name) == 0 &&
           strcmp(p->password, input_pwd) == 0)
        {
            printf("登录成功！欢迎%s\n", p->user_name);
            return p; //返回登录成功的用户节点
        }
        p = p->next;
    }
    printf("用户名或密码错误！\n");
    return NULL; //登录失败
}
//用户借阅书籍
//user: 当前登录的用户节点  book_head: 书籍链表头
void borrow_book(read_user *user, book_list book_head)
{
    char input_book_id[50];
    printf("请输入要借阅的书籍编号：");
    scanf("%s", input_book_id);

    //在书籍链表中查找匹配
    book_list p = book_head->next;
    while(p != NULL)
    {
        if(strcmp(p->book_id, input_book_id) == 0)
        {
            //检查书籍状态
            if(strcmp(p->book_status, "已借阅") == 0)
            {
                printf("该书籍已被借阅，借阅失败！\n");
                return;
            }

            //借阅成功，更新书籍状态
            strcpy(p->book_status, "已借阅");
            time_t t = time(NULL);
            strcpy(p->book_borrow_time, ctime(&t));
            //ctime返回的字符串末尾带换行，去掉它
            p->book_borrow_time[strlen(p->book_borrow_time) - 1] = '\0';

            //把该书加入用户的借阅链表
            book_list borrowed = (book_list)malloc(sizeof(book));
            memcpy(borrowed, p, sizeof(book));
            borrowed->next = NULL;

            if(user->book == NULL)
            {
                user->book = borrowed;
            }
            else
            {
                book_list tail = user->book;
                while(tail->next != NULL)
                    tail = tail->next;
                tail->next = borrowed;
            }

            //保存借阅记录到文件
            save_borrow_to_file(user->user_name, borrowed);

            //更新书籍状态文件
            save_book_status(book_head);

            printf("借阅成功！\n");
            return;
        }
        p = p->next;
    }
    printf("未找到该书籍，借阅失败！\n");
}

//归还书籍
//user: 当前登录的用户节点  book_head: 全局书籍链表头
void return_book(read_user *user, book_list book_head)
{
    char input_book_id[50];
    printf("请输入要归还的书籍编号：");
    scanf("%s", input_book_id);

    //在用户的借阅链表中查找
    book_list p = user->book;
    book_list prev = NULL;
    while(p != NULL)
    {
        if(strcmp(p->book_id, input_book_id) == 0)
        {
            //从用户借阅链表中删除该节点
            if(prev == NULL)
                user->book = p->next;
            else
                prev->next = p->next;
            free(p);

            //更新全局书籍链表中对应书籍的状态为可借阅
            book_list bp = book_head->next;
            while(bp != NULL)
            {
                if(strcmp(bp->book_id, input_book_id) == 0)
                {
                    strcpy(bp->book_status, "可借阅");
                    strcpy(bp->book_borrow_time, "");
                    break;
                }
                bp = bp->next;
            }

            //从文件删除借阅记录
            remove_borrow_from_file(user->user_name, input_book_id);

            //更新书籍状态文件
            save_book_status(book_head);

            printf("归还成功！\n");
            return;
        }
        prev = p;
        p = p->next;
    }
    printf("未找到该借阅记录，归还失败！\n");
}
//显示书籍信息
void display_book_info(read_user *user)
{
    book_list p = user->book;
    if(p == NULL)
    {
        printf("当前没有借阅书籍！\n");
        return;
    }
    printf("当前借阅书籍信息如下：\n");
    while(p != NULL)
    {
        printf("书籍编号：%s\n", p->book_id);
        printf("书籍名字：%s\n", p->book_name);
        printf("书籍类别：%s\n", p->book_type);
        printf("书籍年份：%d\n", p->book_year);
        printf("书籍作者：%s\n", p->book_author);
        printf("借阅状态：%s\n", p->book_status);
        printf("借阅时间：%s\n", p->book_borrow_time);
        printf("归还时间：%s\n", p->book_return_time);
        printf("-----------------\n");
        p = p->next;
    }
}



//用户功能菜单
void user_gn_menu(read_user *user, book_list book_head)
{
    while(1)
    {
        int n;
        printf("=== 用户功能菜单 ===\n");
        printf("1.借阅书籍\n");
        printf("2.归还书籍\n");
        printf("3.显示书籍信息\n");
        printf("4.退出\n");
        int ret = scanf("%d",&n);
        if(ret != 1)
        {
            printf("输入错误，请重新输入\n");
            while(getchar() != '\n');
            continue;
        }
        switch(n)
        {
            case 1:
                borrow_book(user, book_head);
                break;
            case 2:
                return_book(user, book_head);
                break;
            case 3:
                display_book_info(user);
                break;
            case 4:
                return;
            default:
                printf("输入错误，请重新输入\n");
                break;
        }
    }
}



////用户入口
void user_d(user_list head, book_list book_head)
{
    while(1)
    {
        printf("\n-- 用户界面 --\n");
        printf("1.用户登录\n");
        printf("2.用户注册\n");
        printf("3.返回主菜单\n");
        printf("请选择：");

        int n;
        int ret = scanf("%d", &n);
        if(ret != 1)
        {
            printf("输入错误，请重新输入\n");
            while(getchar() != '\n');
            continue;
        }

        switch(n)
        {
            case 1:
            {
                read_user *user = user_login(head);
                if(user != NULL) //登录成功
                {
                    user_gn_menu(user, book_head);
                }
                break;
            }
            case 2:
                user_register(head, book_head); //用户注册
                break;
            case 3:
                return;
            default:
                printf("输入错误，请重新输入\n");
                break;
        }
    }
}

//管理员登录
int admin_login()
{
    char admin_name[20];
    char admin_password[20];
    printf("请输入管理员用户名：");
    scanf("%s", admin_name);
    printf("请输入管理员密码：");
    scanf("%s", admin_password);
    if(strcmp(admin_name, "KOHJU") == 0 && strcmp(admin_password, "kohju123") == 0)
    {
        printf("管理员登录成功！\n");
        return 1;
    }
    else
    {
        printf("管理员登录失败！\n");
        return 0;
    }
}
//管理员：录入图书
void admin_add_book(book_list book_head)
{
    char id[50], name[50], author[50], type[50];
    int year;

    printf("请输入书籍编号：");
    scanf("%s", id);
    printf("请输入书籍名称：");
    scanf("%s", name);
    printf("请输入书籍作者：");
    scanf("%s", author);
    printf("请输入书籍类别：");
    scanf("%s", type);
    printf("请输入出版年份：");
    scanf("%d", &year);

    book_list node = (book_list)malloc(sizeof(book));
    strcpy(node->book_id, id);
    strcpy(node->book_name, name);
    strcpy(node->book_author, author);
    strcpy(node->book_type, type);
    node->book_year = year;
    strcpy(node->book_status, "可借阅");
    strcpy(node->book_borrow_time, "");
    strcpy(node->book_return_time, "");
    node->next = NULL;

    //尾插到链表
    book_list tail = book_head;
    while(tail->next != NULL)
        tail = tail->next;
    tail->next = node;

    //追加到books.txt
    FILE *fp = fopen("books.txt", "a");
    if(fp != NULL)
    {
        fprintf(fp, "%s\t%s\t%s\t%d\n", id, name, author, year);
        fclose(fp);
    }

    save_book_status(book_head);
    printf("录入成功！\n");
}

//管理员：删除图书
void admin_delete_book(book_list book_head)
{
    char id[50];
    printf("请输入要删除的书籍编号：");
    scanf("%s", id);

    book_list prev = book_head;
    book_list p = book_head->next;
    while(p != NULL)
    {
        if(strcmp(p->book_id, id) == 0)
        {
            prev->next = p->next;
            free(p);
            save_book_status(book_head);
            printf("删除成功！\n");
            return;
        }
        prev = p;
        p = p->next;
    }
    printf("未找到该书籍！\n");
}

//管理员：更新图书信息
void admin_update_book(book_list book_head)
{
    char id[50];
    printf("请输入要更新的书籍编号：");
    scanf("%s", id);

    book_list p = book_head->next;
    while(p != NULL)
    {
        if(strcmp(p->book_id, id) == 0)
        {
            printf("当前信息：%s %s %s %d\n",
                   p->book_id, p->book_name, p->book_author, p->book_year);
            printf("请输入新名称：");
            scanf("%s", p->book_name);
            printf("请输入新作者：");
            scanf("%s", p->book_author);
            printf("请输入新类别：");
            scanf("%s", p->book_type);
            printf("请输入新年份：");
            scanf("%d", &p->book_year);

            save_book_status(book_head);
            printf("更新成功！\n");
            return;
        }
        p = p->next;
    }
    printf("未找到该书籍！\n");
}

//管理员：显示所有书籍状态
void admin_show_all_books(book_list book_head)
{
    book_list p = book_head->next;
    if(p == NULL)
    {
        printf("暂无书籍信息！\n");
        return;
    }

    printf("%-8s %-16s %-10s %-6s %-8s %s\n",
           "编号", "名称", "作者", "年份", "状态", "借阅时间");
    printf("--------------------------------------------------------\n");
    while(p != NULL)
    {
        printf("%-8s %-16s %-10s %-6d %-8s %s\n",
               p->book_id, p->book_name, p->book_author,
               p->book_year, p->book_status,
               strlen(p->book_borrow_time) > 0 ? p->book_borrow_time : "无");
        p = p->next;
    }
}

//进入管理员功能菜单
void admin_gn_menu(user_list head, book_list book_head)
{
    while(1)
    {
        printf("\n=== 管理员功能菜单 ===\n");
        printf("1.图书信息录入\n");
        printf("2.图书信息删除\n");
        printf("3.图书信息更新\n");
        printf("4.显示所有书籍状态\n");
        printf("5.退出\n");
        printf("请选择：");

        int n;
        int ret = scanf("%d", &n);
        if(ret != 1)
        {
            printf("输入错误，请重新输入\n");
            while(getchar() != '\n');
            continue;
        }

        switch(n)
        {
            case 1:
                admin_add_book(book_head);
                break;
            case 2:
                admin_delete_book(book_head);
                break;
            case 3:
                admin_update_book(book_head);
                break;
            case 4:
                admin_show_all_books(book_head);
                break;
            case 5:
                return;
            default:
                printf("输入错误，请重新输入\n");
                break;
        }
    }
}


void admin_d(user_list head, book_list book_head)
{
    while(1)
    {
        printf("管理员登录\n");
        printf("1.管理员登录\n");
        printf("2.返回主菜单\n");
        printf("请选择：");
        int n;
        int ret = scanf("%d", &n);
        if(ret != 1)
        {
            printf("输入错误，请重新输入\n");
            while(getchar() != '\n');
            continue;
        }
        switch(n)
        {
            case 1:
                if(admin_login())
                {
                    admin_gn_menu(head, book_head);
                }
                break;
            case 2:
                return;
            default:
                printf("输入错误，请重新输入\n");
                break;
        }
    }
}