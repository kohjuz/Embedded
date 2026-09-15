#ifndef MYHEAD_H
#define MYHEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>

//链表结点，保存每个客户端信息
typedef struct ClientNode
{
    int sockfd;
    char ip[32];
    int port;
    struct ClientNode *next;
}ClientNode;

extern ClientNode *head;//头节点
extern pthread_mutex_t mutex;

void delete_node(ClientNode **head,ClientNode *del);
void *send_msg(void *arg);
void *recv_msg(void *arg);

#endif