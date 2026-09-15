#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stddef.h>

#define DEV_TYPE_LEN 16   /* 设备类型名长度，如 "ir_ac" "mqtt_light" */
#define DEV_MAX      16   /* V0.1 设备表容量，够用即可，不搞动态扩容 */

struct device;

/* 统一操作接口：所有设备只有三个动作。
   品牌差异全部压在 .c 驱动里，上层永远只看到这三个函数指针。 */
typedef int (*dev_op_t)(struct device *dev);
typedef int (*dev_dump_t)(struct device *dev, char *buf, size_t n);

typedef struct device {
    int         id;                     /* 全局唯一编号 */
    char        type[DEV_TYPE_LEN];     /* 设备类型，驱动据此分流 */
    int         state;                  /* 0=关 1=开 */
    void       *priv;                   /* 驱动私有数据（如 NEC 码值） */
    dev_op_t    on;                     /* 开机 */
    dev_op_t    off;                    /* 关机 */
    dev_dump_t  dump;                   /* 状态转字符串，给 UI/日志用 */
} device_t;

/* 设备注册表：整个合界的核心只有这一张表 */
int      dev_register(device_t *dev);   /* 成功 0，表满/空指针 -1 */
device_t *dev_find(int id);             /* 找不到返回 NULL */
void     dev_list(void);                /* 打印所有设备 */

#endif
