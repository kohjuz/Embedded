/*
 * 精简版内核双向循环链表 (kernel_list.h)
 * 基于 Linux 内核 include/linux/list.h
 * 仅保留标准双向链表 (struct list_head)，去除了哈希链表 (hlist)
 */

#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

#include <stddef.h> // 用于 offsetof

// ---------- 核心数据结构 ----------
struct list_head {
    struct list_head *next, *prev;
};

// ---------- 辅助宏：获取结构体首地址 ----------
// 经典 container_of 宏，用于从成员指针获取整个结构体的指针  
/* 通过计算已知地址前面元素的偏移量得出大结构体的地址
ptr 指向已知成员的指针(即已知地址)  type (大结构的的类型)  member (已知成员名称)*/
#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

// ---------- 初始化 ----------
#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(list) do { \
    (list)->next = (list);        \
    (list)->prev = (list);        \
}while(0)

// ---------- 插入操作 ----------
// 在 head 和 head->next 之间插入新节点 newNodeNode
static inline void __list_add(struct list_head *newNode,struct list_head *prev,struct list_head *next) 
{
    next->prev = newNode;
    newNode->next = next;
    newNode->prev = prev;
    prev->next = newNode;
}

// 头插法：在 head 节点之后插入 newNode
static inline void list_add(struct list_head *newNode, struct list_head *head) {
    __list_add(newNode, head, head->next);
}

// 尾插法：在 head 节点之前插入 newNode
static inline void list_add_tail(struct list_head *newNode, struct list_head *head) {
    __list_add(newNode, head->prev, head);
}

// ---------- 删除操作 ----------
// 删除 prev 和 next 之间的节点
static inline void __list_del(struct list_head *prev, struct list_head *next) {
    next->prev = prev;
    prev->next = next;
}

// 从链表中删除 entry 节点
static inline void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    // 可选：将删除的节点指针置为特殊值，防止悬空指针使用
    // entry->next = (struct list_head *)0xdead;
    // entry->prev = (struct list_head *)0xbeef;
}

// ---------- 判断操作 ----------
// 判断链表是否为空（仅包含头结点）
static inline int list_empty(const struct list_head *head) {
    return head->next == head;
}

// ---------- 获取结构体指针 ----------
// 核心宏：从链表节点指针获取包含它的结构体指针
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

//----------- 移动节点 ---------------
// 将节点list，移动到指定位置head的后面
static inline void list_move(struct list_head *list,
                             struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

// 将节点list，移动到指定位置head的前面
static inline void list_move_tail(struct list_head *list,
                                  struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

// ---------- 遍历操作 ----------
// 基础遍历：pos 依次指向链表中的每个 list_head 节点
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

// 安全遍历：用于遍历时删除节点，n 用于暂存下一个节点
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

// 高级遍历：直接获取包含 list_head 的自定义结构体指针 pos
#define list_for_each_entry(pos, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof(*pos), member))

// 安全的高级遍历：用于遍历时删除节点
#define list_for_each_entry_safe(pos, n, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member), \
         n = list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = list_entry(n->member.next, typeof(*n), member))

#endif // _LINUX_LIST_H