#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "head.h"

struct msgbuf 
{
    long mtype;
    char mtext[1024];
};

/* 吃掉 stdin 里剩下的字符直到换行，解决 scanf 后 fgets 读空行的问题 */
static void eat_line(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(void)
{
    /* ftok 要求 /home/dell/tmp 文件必须存在，运行前先: touch /home/dell/tmp */
    key_t key = ftok("/home/dell/tmp", 1);
    if (key == -1) { perror("ftok"); return 1; }

    int msg_qid = msgget(key, IPC_CREAT | 0666);
    if (msg_qid == -1) { perror("msgget"); return 1; }

    struct msgbuf msg;
    int n;
    while(1)
    {
    printf("发送数据(12) 还是 接收数据(21)？\n");
    fflush(stdout);
    if (scanf("%d", &n) != 1) 
    {
        printf("输入错误\n");
        return 1;
    }
    eat_line();

    if (n == 12) 
    {                       /* P1 -> P2 */
        printf("请输入要发送的内容：\n");
        fflush(stdout);
        if (fgets(msg.mtext, sizeof(msg.mtext), stdin) == NULL) 
        {
            perror("fgets");
            return 1;
        }
        msg.mtext[strcspn(msg.mtext, "\n")] = '\0';   /* 去掉 fgets 带来的换行 */
        msg.mtype = P1_P2;

        if (msgsnd(msg_qid, &msg, strlen(msg.mtext) + 1, 0) == -1) 
        {
            perror("msgsnd");
            return 1;
        }
        printf("p1 已发送：%s\n", msg.mtext);
    }
    else if (n == 21) 
    {                  /* P1 <- P2 */
        if (msgrcv(msg_qid, &msg, sizeof(msg.mtext), P2_P1, 0) == -1) 
        {
            perror("msgrcv");
            return 1;
        }
        printf("p1 收到消息：%s\n", msg.mtext);
    }
    else
     {
        printf("输入错误\n");
        return 1;
    }
}

   
     
    return 0;
}
