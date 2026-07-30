#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct node
{
    int data;
    struct node *next;
}node;

node *init_list(void)
{
    return NULL;
}

node *new_node(int data)
{
    node *new = malloc(sizeof(node));
    if(new != NULL)
    {
        new->data = data;
        new->next = NULL;
    }
    return new;
}

// void list_add_tail(node **phead, node *new)
// {
//     // 0.
//     if(*phead == NULL)
//         *phead = new;

//     // 1.
//     node *p;
//     for(p=*phead; p->next!=NULL; p=p->next);

//     // 2.
//     p->next = new;
// }

node *list_add_tail(node *head, node *new)
{
    // 0.
    if(head == NULL)
        return new;

    // 1.
    node *p;
    for(p=head; p->next!=NULL; p=p->next);

    // 2.
    p->next = new;
    return head;
}

void show_list(node *head)
{
    if(head == NULL)
        printf("链表为空\n");

    for(node *p=head; p!=NULL; p=p->next)
        printf("%d\t", p->data);
    printf("\n");
}

node *list_del(node *head, node *p)
{
    if(head == NULL || p == NULL)
        return NULL;

    if(p == head)
    {
        node *tmp = head->next;
        head->next = NULL;
        return tmp;
    }

    node *k;
    for(k=head; k!=NULL && k->next!=p; k=k->next);
    k->next = p->next;
    p->next = NULL;

    return head;
}

int main(int argc, char const *argv[])
{
    // 1. 搞个空链表
    node *head = init_list();

    // 2. 放入若干随机数
    srand(time(NULL));
    for(int i=0; i<10; i++)
    {
        node *new = new_node(rand()%1000);
        head = list_add_tail(head, new);
    }
    show_list(head);

    // 3. 排好序输出
    node *head2 = init_list();

    node *min=head, *p;
    while(head != NULL)
    {
        min=head;
        for(p=head; p!=NULL; p=p->next)
        {
            if(p->data < min->data)
                min = p;
        }

        head = list_del(head, min);

        head2 = list_add_tail(head2, min);
    }
    show_list(head);
    show_list(head2);

    return 0;
}
