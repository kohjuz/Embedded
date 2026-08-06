#include <stdio.h>
#include <stdlib.h>

struct node
{
    int n;
    struct node *left;
    struct node *right;
};

struct node *new_node(int n)
{
    struct node *node = (struct node *)malloc(sizeof(struct node));
    if(node != NULL)
    {
        node->n = n;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

int compare(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

struct node *build_balanced_bst_internal(int arr[], int left, int right, int is_top)
{
    if(left > right)
        return NULL;

    int len = right - left + 1;
    int mid;

    if(is_top && len % 2 == 0)
        // 顶层偶数长度：取中间偏左
        mid = (left + right) / 2;
    else if(len % 2 == 0)
        // 子数组偶数长度：取中间偏右
        mid = (left + right + 1) / 2;
    else
        // 奇数长度：取中间
        mid = (left + right) / 2;

    struct node *root = new_node(arr[mid]);

    root->left = build_balanced_bst_internal(arr, left, mid - 1, 0);
    root->right = build_balanced_bst_internal(arr, mid + 1, right, 0);

    return root;
}

struct node *build_balanced_bst(int arr[], int left, int right)
{
    return build_balanced_bst_internal(arr, left, right, 1);
}

// 前序遍历
void pre_travel(struct node *root)
{
    if(root == NULL)
        return;

    // 1. 访问根节点
    printf("%d ", root->n);

    // 2. 访问左子树
    pre_travel(root->left);

    // 3. 访问右子树
    pre_travel(root->right);
}

// 中序遍历
void in_travel(struct node *root)
{
    if(root == NULL)
        return;

    // 1. 访问左子树
    in_travel(root->left);

    // 2. 访问根节点
    printf("%d ", root->n);

    // 3. 访问右子树
    in_travel(root->right);
}

// 后序遍历
void post_travel(struct node *root)
{
    if(root == NULL)
        return;

    // 1. 访问左子树
    post_travel(root->left);

    // 2. 访问右子树
    post_travel(root->right);

    // 3. 访问根节点
    printf("%d ", root->n);
}
// 层序遍历
#define MAX_SIZE 100

struct node_queue
{
    struct node *data[MAX_SIZE];
    int front;
    int rear;
};

void queue_init(struct node_queue *q)
{
    q->front = 0;
    q->rear = 0;
}

int queue_is_empty(struct node_queue *q)
{
    return q->front == q->rear;
}

int queue_push(struct node_queue *q, struct node *val)
{
    if((q->rear + 1) % MAX_SIZE == q->front)
        return 0;
    q->data[q->rear] = val;
    q->rear = (q->rear + 1) % MAX_SIZE;
    return 1;
}

struct node *queue_pop(struct node_queue *q)
{
    if(queue_is_empty(q))
        return NULL;
    struct node *val = q->data[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    return val;
}

void level_travel(struct node *root)
{
    if(root == NULL)
        return;

    struct node_queue q;
    queue_init(&q);

    queue_push(&q, root);

    while(!queue_is_empty(&q))
    {
        struct node *cur = queue_pop(&q);
        printf("%d ", cur->n);

        if(cur->left != NULL)
            queue_push(&q, cur->left);
        if(cur->right != NULL)
            queue_push(&q, cur->right);
    }
}




int main(void)
{
    int arr[100];
    int count = 0;

    int n;
    while(1)
    {
        scanf("%d", &n);
        if(n == 0)
            break;
        arr[count++] = n;
    }

    qsort(arr, count, sizeof(int), compare);

    struct node *root = build_balanced_bst(arr, 0, count - 1);

    printf("输入数据（排序后）：");
    for(int i = 0; i < count; i++)
        printf("%d ", arr[i]);
    printf("\n");

    // 遍历二叉树

    // 1. 前序遍历
    printf("前序遍历：");
    pre_travel(root); printf("\n");

    // 2. 中序遍历
    printf("中序遍历：");
    in_travel(root); printf("\n");
    // 3. 后序遍历
    printf("后序遍历：");
    post_travel(root); printf("\n");

    // 4. 层序遍历
    printf("层序遍历：");
    level_travel(root); printf("\n");

    return 0;
}
