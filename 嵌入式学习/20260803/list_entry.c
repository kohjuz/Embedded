#include <stdio.h>
#include <stdint.h>

struct list_head list
{
    struct list_head next;
    struct list_head prev;
};
struct node
{
    int data;

    char name[10];
    float score;
    double height;
    unsigned int age;
    unsigned short sex;
    unsigned char id[10];
    struct list_head *list;
};
char a;
struct node node1={1,2.0,3.0,4,5,1,&node1.list};

struct node *node1=&node1->list;



void list_entry()
{
    struct node *p=(struct node *)(char *)(list_a)-(uint64_t)(&(((struct node *) 0x00)->list)));
   printf("%d\n",p->data);
   printf("%s\n",p->name);
   printf("%f\n",p->score);
   printf("%lf\n",p->height);
   printf("%d\n",p->age);
   printf("%d\n",p->sex);
   printf("%s\n",p->id);
   printf("%s\n",p->address);   
}