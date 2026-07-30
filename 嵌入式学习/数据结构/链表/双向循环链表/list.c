#include <stdio.h>

typedef struct a
{
    char name[10];
    int age;
}a;

#define DATATYPE a
#include "list.h"

void show_a(a a)
{
    printf("%s(%d) ", a.name, a.age);
}

int compare_a(a a1, a a2)
{
    return strcmp(a1.name, a2.name);
}

int main(int argc, char *argv[])
{
    dlist head = init_list();

    printf("=== 1. 测试空链表 ===\n");
    printf("是否为空: %s\n", list_is_empty(head) ? "是" : "否");
    printf("长度: %d\n", list_length(head));

    printf("\n=== 2. 尾部插入 ===\n");
    a a1 = {"张三", 18};
    a a2 = {"李四", 19};
    a a3 = {"王五", 20};
    list_add_tail(head, new_node(a1));
    list_add_tail(head, new_node(a2));
    list_add_tail(head, new_node(a3));
    printf("尾部插入3个节点后: ");
    show_list(head, show_a);

    printf("\n=== 3. 头部插入 ===\n");
    a a0 = {"赵六", 17};
    list_add_head(head, new_node(a0));
    printf("头部插入\"赵六\"后: ");
    show_list(head, show_a);
    printf("长度: %d\n", list_length(head));

    printf("\n反向遍历\n");
    printf("反向输出: ");
    show_list_reverse(head, show_a);

    printf("\n查找节点\n");
    a search = {"李四", 0};
    dlist found = list_find(head, search, compare_a);
    if(found != NULL)
        printf("找到: %s(%d)\n", found->data.name, found->data.age);
    else
        printf("未找到\n");

    a notfound = {"孙七", 0};
    found = list_find(head, notfound, compare_a);
    if(found != NULL)
        printf("找到: %s(%d)\n", found->data.name, found->data.age);
    else
        printf("查找\"孙七\": 未找到\n");

    printf("\n=== 6. 删除节点 ===\n");
    a del = {"王五", 0};
    dlist target = list_find(head, del, compare_a);
    if(target != NULL)
    {
        printf("删除\"王五\"前: ");
        show_list(head, show_a);
        list_remove(head, target);
        printf("删除\"王五\"后: ");
        show_list(head, show_a);
        printf("长度: %d\n", list_length(head));
    }

    printf("\n=== 7. 清空链表 ===\n");
    delete_list(head, NULL);
    printf("链表已释放\n");

    return 0;
}
