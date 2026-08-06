// 哈希表、散列表

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#define SIZE 20

typedef int datatype;

struct node
{
    datatype data;
    struct node *next;
};

typedef struct
{
    unsigned long table_size;
    struct node **table_entry;

}hash_table;

void show(hash_table *ht, unsigned long pos, datatype data);

hash_table *init_ht(unsigned long size)
{
    // 哈希表的管理结构体
    hash_table *ht = malloc(sizeof(hash_table));
    ht->table_size = size;

    // 哈希表（数组）
    ht->table_entry = calloc(size, sizeof(struct node *));

    return ht;
}

void hash_add(datatype data, hash_table *ht)
{
    // 使用保留除数法，获得哈希地址（即数组的下标值）
    unsigned long hash_addr = data % (SIZE-1);

    struct node *new = malloc(sizeof(struct node));
    new->data = data;
    new->next = NULL;

    show(ht, hash_addr, data);
    printf("=================================\n");

    // 1：该哈希地址可用，直接将新节点放进去
    if(ht->table_entry[hash_addr] == NULL)
    {
        ht->table_entry[hash_addr] = new;
    }
    // 2：该哈希地址不可用，将新节点链到冲突链表的末尾
    else
    {
        struct node *p = ht->table_entry[hash_addr];
        while(p->next != NULL)
        {
            p = p->next;
        }
        p->next = new;
    }
}

void show(hash_table *ht, unsigned long pos, datatype data)
{
    struct node *p;

    int i;
    for(i=0; i<ht->table_size; i++)
    {
        p = ht->table_entry[i];

        printf("table_entry[%d]: ", i);

        if(p != NULL)
        {
            struct node *q = p;
            while(q != NULL)
            {
                printf("%d\t", q->data);
                q = q->next;
            }
        }
    
        if(pos == i)
        {
            printf("\t <-- %d\n", data);
        }
        else
        {
            printf("\n");
        }
    }
}
    // 在原有哈希表的基础上，填充查表的测试代码
    // 查表,输入一个数能查到他的位置和查找次数
    //定义一个数据，用来计数查找的次数count
    //把他封装起来能查多次，每次查完后，count重置为0

void hash_search(hash_table *ht)
{
    int count = 0;
    int data = 0;
    printf("请输入要查找的数：");
    scanf("%d", &data); 
    unsigned long hash_addr = data % (SIZE-1);
    struct node *p = ht->table_entry[hash_addr];
    while(p != NULL)
    {
        count = 0;
        int data = 0;
        printf("请输入要查找的数：");
        scanf("%d", &data); 
        unsigned long hash_addr = data % (SIZE-1);
        struct node *p = ht->table_entry[hash_addr];
        while(p != NULL)
        {
            count++;
        if(p->data == data)
        {
            printf("数据：%d 位于 table_entry[%d]\n", data, hash_addr);
            
            printf("查找次数： %d\n", count);
            break;
        }
        else
        {
            p = p->next;
        }
    }
    if(p == NULL)
    {
        printf("数据：%d 未找到\n", data);
        printf("查找次数： %d\n", count);
    }
}
}

int main(void)
{
    // 哈希表的初始化
    hash_table *ht = init_ht(SIZE);

    // 使用除留余数法/冲突链表的形式，造表
    srand(time(NULL));
    int i;
    for(i=0; i<10; i++)
    {
        hash_add(rand()%1000, ht);
        sleep(1);
    }
    show(ht, -1, -1);// 显示哈希表

    hash_search(ht);



    // ....

    return 0;
}