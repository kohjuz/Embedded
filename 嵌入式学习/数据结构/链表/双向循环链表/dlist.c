#include <stdio.h>
#include <stdlib.h>



typedef struct node
{
    int data;
    struct node *prev;
    struct node *next;
}node,*dlist;

dlist init_list()
{
    dlist head = (dlist)malloc(sizeof(node));
    if(head == NULL)
    {
        printf("malloc failed\n");
        return NULL;
    }
    head->prev = head;
    head->next = head;
    return head;
}
dlist new_node(int data)
{
    dlist node = (dlist)malloc(sizeof(node));
    if(node == NULL)
    {
        printf("malloc failed\n");
        return NULL;
    }
    node->data = data;
    node->prev = node;
    node->next = node;
    return node;
}
void list_add_tail(dlist head,dlist node)
{
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
    node->next = head;
}
void show_list(dlist head)
{
    dlist node = head->next;
    while(node != head)
    {
        printf("%d ",node->data);
        node = node->next;
    }
    printf("\n");
}
void delete_list(dlist head)
{
    dlist node = head->next;
    while(node != head)
    {
        dlist next = node->next;
        free(node);
        node = next;
    }
    free(head);
}
int main(int argc, char const *argv[])
{
    dlist head =init_list();
    for(int i=0;i<10;i++)
    {
        list_add_tail(head,new_node(i));
    }
    show_list(head);

    delete_list(head);
    return 0;
}
