#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "head.h"

struct msgbuf {
    long mtype;
    char mtext[1024];
};

static void eat_line(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(void)
{
    key_t key = ftok("/home/dell/tmp", 1);
    if (key == -1) { perror("ftok"); return 1; }

    int msg_qid = msgget(key, IPC_CREAT | 0666);
    if (msg_qid == -1) { perror("msgget"); return 1; }

    struct msgbuf msg;
    int n;

    while (1) {
        printf("接收数据(12) / 发送数据(21) / 退出(0)：\n");
        fflush(stdout);
        if (scanf("%d", &n) != 1) 
        {
            printf("输入错误\n");
            eat_line();
            continue;
        }
        eat_line();

        if (n == 12) 
        {                   /* P2 接收 P1 */
            if (msgrcv(msg_qid, &msg, sizeof(msg.mtext), P1_P2, 0) == -1) 
            {
                perror("msgrcv");
                continue;
            }
            printf("p2 收到消息：%s\n", msg.mtext);
        }
        else if (n == 21) 
        {              /* P2 发送 P1 */
            printf("请输入要发送的内容：\n");
            fflush(stdout);
            if (fgets(msg.mtext, sizeof(msg.mtext), stdin) == NULL) 
            {
                perror("fgets");
                continue;
            }
            msg.mtext[strcspn(msg.mtext, "\n")] = '\0';
            msg.mtype = P2_P1;

            if (msgsnd(msg_qid, &msg, strlen(msg.mtext) + 1, 0) == -1)
             {
                perror("msgsnd");
                continue;
            }
            printf("p2 已发送：%s\n", msg.mtext);
        }
        else if (n == 0) 
        {
            break;
        }
        else 
        {
            printf("输入错误\n");
        }
    }

    
    if (msgctl(msg_qid, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl IPC_RMID");
    }
    return 0;
}
