
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
void handler(int sig)
{
    printf("收到信号：%d\n",sig);

}
void homework_help(int homework,void(*f)())
{
    printf("自动写作业:%d\n",homework);
    f();
}
void Feiq()
{
    printf("飞秋提交");

}
void em()
{
    printf("邮件提交\n");
}

int main(int argc,char const argv[])
{
    signal(SIGINT,handler);
    homework_help(123,em);
    homework_help(456,Feiq);
  

    return 0;

}