#include <stdio.h>
#include "device.h"

/* 设备表：静态数组，不做动态内存，V0.1 先跑通闭环 */
static device_t *g_table[DEV_MAX];
static int       g_count;

int dev_register(device_t *dev)
{
    if (dev == NULL || g_count >= DEV_MAX)
        return -1;

    g_table[g_count++] = dev;
    return 0;
}

device_t *dev_find(int id)
{
    for (int i = 0; i < g_count; i++) {
        if (g_table[i]->id == id)
            return g_table[i];
    }
    return NULL;
}

void dev_list(void)
{
    char buf[64];

    printf("  共 %d 个设备：\n", g_count);
    for (int i = 0; i < g_count; i++) {
        device_t *d = g_table[i];
        if (d->dump)
            d->dump(d, buf, sizeof buf);
        else
            snprintf(buf, sizeof buf, "%s", d->type);

        printf("   [%d] %s\n", d->id, buf);
    }
}
