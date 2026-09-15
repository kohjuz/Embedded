#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "head.h"

struct msgbuf {
    long mtype;
    char mtext[1024];
};

/* 把一行文本塞进消息并发送（自动补 '\0'，长度安全） */
static int send_line(int qid, long mtype, const char *text)
{
    struct msgbuf msg;
    msg.mtype = mtype;
    strncpy(msg.mtext, text, sizeof(msg.mtext) - 1);
    msg.mtext[sizeof(msg.mtext) - 1] = '\0';
    return msgsnd(qid, &msg, strlen(msg.mtext) + 1, 0);
}

int main(int argc, char *argv[])
{
    if (argc != 2 || (argv[1][0] != 'A' && argv[1][0] != 'B')) {
        fprintf(stderr, "用法:\n");
        fprintf(stderr, "  终端1: %s A   (A 先发)\n", argv[0]);
        fprintf(stderr, "  终端2: %s B   (B 先收)\n", argv[0]);
        return 1;
    }
    int isA = (argv[1][0] == 'A');

    /* ftok 依赖的文件必须真实存在！先在 Linux 侧执行: touch /home/dell/tmp */
    key_t key = ftok("/home/dell/tmp", 1);
    if (key == -1) { perror("ftok"); return 1; }

    int qid = msgget(key, IPC_CREAT | 0666);
    if (qid == -1) { perror("msgget"); return 1; }

    /* 角色映射：A 发 P1_P2 / 收 P2_P1；B 反过来 */
    long my_send = isA ? P1_P2 : P2_P1;
    long my_recv = isA ? P2_P1 : P1_P2;

    struct msgbuf msg;
    char line[1024];

    printf("[%s] 聊天开始，输入 quit 退出\n", isA ? "A" : "B");
    fflush(stdout);

    while (1) {
        if (isA) {
            /* A: 先发后收 —— 严格一来一回，不会两边都卡在 msgrcv */
            printf("A> "); fflush(stdout);
            if (!fgets(line, sizeof(line), stdin)) break;
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, "quit") == 0) { send_line(qid, my_send, "quit"); break; }
            if (send_line(qid, my_send, line) == -1) { perror("msgsnd"); break; }

            if (msgrcv(qid, &msg, sizeof(msg.mtext), my_recv, 0) == -1) { perror("msgrcv"); break; }
            if (strcmp(msg.mtext, "quit") == 0) break;
            printf("B> %s\n", msg.mtext);
        } else {
            /* B: 先收后发 */
            if (msgrcv(qid, &msg, sizeof(msg.mtext), my_recv, 0) == -1) { perror("msgrcv"); break; }
            if (strcmp(msg.mtext, "quit") == 0) break;
            printf("A> %s\n", msg.mtext);

            printf("B> "); fflush(stdout);
            if (!fgets(line, sizeof(line), stdin)) break;
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(line, "quit") == 0) { send_line(qid, my_send, "quit"); break; }
            if (send_line(qid, my_send, line) == -1) { perror("msgsnd"); break; }
        }
    }

    /* 退出时尝试删队列（对方可能已删，忽略错误） */
    msgctl(qid, IPC_RMID, NULL);
    printf("已退出。\n");
    return 0;
}
