#ifndef __LIST_TEST_H
#define __LIST_TEST_H

#include<stdlib.h>

typedef struct LNode
{
    int data;
    struct LNode *next;

}LNode;
LNode *list_init(void);
LNode *new_node(int data);

void add_node(LNode *head,LNode *node);
void add_node_tail(LNode *head,LNode *node);
void show(LNode *head);
void show_tail(LNode *head);
void destroy(LNode *head);
void reverse_list(LNode *head);

#endif