#ifndef __DLIST_H
#define __DLIST_H

/* This file is from Linux Kernel (include/linux/list.h)
* and modified by simply removing hardware prefetching of list items.
* Here by copyright, credits attributed to wherever they belong.
* Kulesh Shanmugasundaram (kulesh [squiggly] isis.poly.edu)
*/

/*
* Simple doubly linked list implementation.
*
* Some of the internal functions (“__xxx”) are useful when
* manipulating whole lists rather than single entries, as
* sometimes we already know the next/prev entries and we can
* generate better code by using them directly rather than
* using the generic single-entry routines.
*/

/**
 * offsetof - 计算结构体成员相对于结构体首地址的字节偏移
 * @TYPE: 结构体类型
 * @MEMBER: 结构体内部成员名
 */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * container_of - 通过结构体成员指针反推得到整个外层结构体首地址
 * @ptr:	指向结构体内部成员的指针
 * @type:	外层完整结构体类型
 * @member:	结构体内部该成员的名称
 */
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})

/*
 * LIST_POISON 非法指针标记
 * 用于标记已经从链表摘除、不再使用的节点，访问会触发缺页异常，辅助排查野指针bug
 */
#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200)

/**
 * struct list_head 通用双向循环链表头/节点
 * 只包含前驱、后继指针，无业务数据
 * 使用方式：内嵌到自定义业务结构体中
 */
struct list_head
{
	struct list_head *prev;
	struct list_head *next;
};

/**
 * LIST_HEAD_INIT - 链表头初始化常量表达式
 * 空双向循环链表：前驱和后继都指向自身
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/**
 * LIST_HEAD - 定义并初始化一个链表头变量
 * 使用示例：LIST_HEAD(my_head);
 */
#define LIST_HEAD(name) \
struct list_head name = LIST_HEAD_INIT(name)

/**
 * INIT_LIST_HEAD - 运行时初始化链表头/节点为空链表
 * do{}while(0) 保证宏可以安全在if/for后单独使用
 */
#define INIT_LIST_HEAD(ptr) \
    do { \
    (ptr)->next = (ptr); \
    (ptr)->prev = (ptr); \
} while (0)

/**
* __list_add 内部函数：在已知的prev、next两个节点之间插入新节点
* new: 待插入节点
* prev: 前一个节点
* next: 后一个节点
* 仅内部调用，外部不要直接使用
*/
static inline void __list_add(struct list_head *new,
				struct list_head *prev,
				struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
* list_add – 头插：在head节点之后插入新节点（适合实现栈）
* @new: 待添加节点
* @head: 链表头
*/
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/**
* list_add_tail – 尾插：在head节点之前插入新节点（适合实现队列）
* @new: 待添加节点
* @head: 链表头
*/
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/**
* __list_del 内部函数：摘除entry节点，把前后节点互相链接
* prev: 前驱节点
* next: 后继节点
* 仅内部调用
*/
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

/**
* list_del – 将节点从链表摘除
* @entry: 需要删除的链表节点
* 注意：摘除后不会初始化节点，节点指针处于非法状态
*/
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = (void *) 0;
	entry->prev = (void *) 0;
}

/**
* list_del_init – 删除节点，并把节点重新初始化为独立空链表
* @entry: 需要删除的节点
*/
static inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

/**
* list_move – 将list节点从原有链表摘下，头插到head之后
* @list: 需要移动的节点
* @head: 目标链表头
*/
static inline void list_move(struct list_head *list,
				struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

/**
* list_move_tail – 将list节点从原有链表摘下，尾插到head之前
* @list: 需要移动的节点
* @head: 目标链表头
*/
static inline void list_move_tail(struct list_head *list,
					struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

/**
* list_empty – 判断链表是否为空
* @head: 链表头
* 返回非0代表空链表；0代表链表存在有效节点
*/
static inline int list_empty(struct list_head *head)
{
	return head->next == head;
}

/**
* __list_splice 内部函数：把list链表所有节点合并到head之后
* list源链表，head目标链表头
*/
static inline void __list_splice(struct list_head *list,
					struct list_head *head)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;
	struct list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/**
* list_splice – 合并两个链表，将list链表接入head后面
* @list: 待合并的源链表
* @head: 目标链表头
* 合并后源list不会被初始化
*/
static inline void list_splice(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head);
}

/**
* list_splice_init – 合并链表，合并完成后将源list重置为空链表
* @list: 待合并源链表
* @head: 目标链表头
*/
static inline void list_splice_init(struct list_head *list,
struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head);
		INIT_LIST_HEAD(list);
	}
}

/**
* list_entry – 通过list_head成员指针获取外层业务结构体指针（老式实现）
* @ptr:    struct list_head成员指针
* @type:    外层结构体类型
* @member:    结构体内部list_head成员名
*/
#define list_entry(ptr, type, member) \
((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
* list_for_each    -    正向遍历所有list_head节点
* @pos:    循环临时变量，struct list_head*
* @head:    链表头
* 注意：遍历过程不要直接删除pos，会导致断链崩溃
*/
#define list_for_each(pos, head) \
for (pos = (head)->next; pos != (head); \
pos = pos->next)

/**
* list_for_each_prev    -    反向遍历list_head节点
* @pos:    struct list_head*循环变量
* @head:    链表头
*/
#define list_for_each_prev(pos, head) \
for (pos = (head)->prev; pos != (head); \
pos = pos->prev)

/**
* list_for_each_safe    -    安全正向遍历list_head，支持遍历中删除节点
* @pos: 当前节点
* @n: 保存下一个节点临时变量
* @head: 链表头
*/
#define list_for_each_safe(pos, n, head) \
for (pos = (head)->next, n = pos->next; pos != (head); \
pos = n, n = pos->next)

/**
* list_for_each_entry    -    直接遍历自定义业务结构体
* @pos:    自定义结构体指针（循环变量）
* @head:    链表头struct list_head*
* @member:    结构体内部list_head成员名字
*/
#define list_for_each_entry(pos, head, member)                \
for (pos = list_entry((head)->next, typeof(*pos), member);    \
&pos->member != (head);                     \
pos = list_entry(pos->member.next, typeof(*pos), member))

/**
* list_for_each_entry_safe – 安全遍历自定义结构体，遍历中允许删除节点
* @pos:    当前业务结构体指针
* @n:        保存下一个业务结构体指针
* @head:    链表头
* @member:    结构体内部list_head成员名
*/
#define list_for_each_entry_safe(pos, n, head, member)            \
for (pos = list_entry((head)->next, typeof(*pos), member),    \
n = list_entry(pos->member.next, typeof(*pos), member);    \
&pos->member != (head);                     \
pos = n, n = list_entry(n->member.next, typeof(*n), member))

#endif