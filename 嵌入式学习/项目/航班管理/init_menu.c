#include <stdio.h>

//本文件用来写航班初始化菜单页面

static void fight_init_menu()
{
    printf("欢迎使用航天管理系统\n");
    while(1)
    {   
        int n=0;
        printf("1.用户登录\n");
        printf("2用户注册\n");
        printf("3管理员登录\n");
        printf("4.退出\n");
        int ret=scanf("%d",&n);
        if(ret!=1)
        {
            printf("输入错误，请正确输入\n");
            while(getchar()='\n');
            continue;
        }
        switch(n)
        {
            case:1
            if(用户登录)
            {
                用户功能菜单

            }
            break;
            case:2
            用户注册
            break;

            case:3
            管理员登录
            break;
        }

        




    }
}