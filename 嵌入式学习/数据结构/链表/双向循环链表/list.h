#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DATATYPE
#define DATATYPE int
#endif

typedef DATATYPE datatype;

typedef struct node
{
    datatype data;
    struct node *prev;
    struct node *next;
}node,*dlist;

dlist init_list()
{
    dlist head = (dlist)malloc(sizeof(struct node));
    if(head == NULL)
    {
        printf("malloc failed\n");
        return NULL;
    }
    head->prev = head;
    head->next = head;
    return head;
}

dlist new_node(datatype data)
{
    dlist node = (dlist)malloc(sizeof(struct node));
    if(node == NULL)
    {
        printf("malloc failed\n");
        return NULL;
    }
    memcpy(&node->data, &data, sizeof(datatype));
    node->prev = node;
    node->next = node;
    return node;
}

int list_is_empty(dlist head)
{
    return head->next == head;
}

int list_length(dlist head)
{
    int count = 0;
    dlist node = head->next;
    while(node != head)
    {
        count++;
        node = node->next;
    }
    return count;
}

void list_add_head(dlist head, dlist node)
{
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
}

void list_add_tail(dlist head, dlist node)
{
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
    node->next = head;
}

void list_remove(dlist head, dlist node)
{
    if(list_is_empty(head) || node == head)
        return;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
}

dlist list_find(dlist head, datatype data, int (*compare)(datatype, datatype))
{
    dlist node = head->next;
    while(node != head)
    {
        if(compare(node->data, data) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}

void show_list(dlist head, void (*f)(datatype))
{
    if(list_is_empty(head))
    {
        printf("(empty list)\n");
        return;
    }
    dlist node = head->next;
    while(node != head)
    {
        f(node->data);
        node = node->next;
    }
    printf("\n");
}

void show_list_reverse(dlist head, void (*f)(datatype))
{
    if(list_is_empty(head))
    {
        printf("(empty list)\n");
        return;
    }
    dlist node = head->prev;
    while(node != head)
    {
        f(node->data);
        node = node->prev;
    }
    printf("\n");
}

void delete_list(dlist head, void (*f)(datatype))
{
    dlist node = head->next;
    while(node != head)
    {
        dlist next = node->next;
        if(f != NULL)
            f(node->data);
        free(node);
        node = next;
    }
    free(head);
}
