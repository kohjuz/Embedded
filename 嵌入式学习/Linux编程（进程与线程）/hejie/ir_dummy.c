#include <stdio.h>
#include <string.h>
#include "device.h"
#include "ir_dummy.h"

#define IR_MAX 8

/* NEC 码帧：地址码 + 命令码。真实时序是 9ms 引导脉冲 + 4.5ms 间隔 + 32bit 数据 */
typedef struct {
    unsigned char addr;      /* 地址码：区分空调 / 电视 */
    unsigned char cmd_on;    /* 开机命令码 */
    unsigned char cmd_off;   /* 关机命令码 */
} ir_priv_t;

static device_t  g_ir_dev[IR_MAX];
static ir_priv_t g_ir_priv[IR_MAX];
static int       g_ir_count;

/* 唯一的硬件耦合点：把码值变成 38kHz 载波脉冲发出去 */
static int ir_send(ir_priv_t *p, unsigned char cmd)
{
    printf("  [IR] 发射 NEC 帧: addr=0x%02X cmd=0x%02X\n", p->addr, cmd);
    return 0;
}

static int ir_on(device_t *d)
{
    ir_priv_t *p = (ir_priv_t *)d->priv;

    ir_send(p, p->cmd_on);
    d->state = 1;
    printf("  -> %s 已开机\n", d->type);
    return 0;
}

static int ir_off(device_t *d)
{
    ir_priv_t *p = (ir_priv_t *)d->priv;

    ir_send(p, p->cmd_off);
    d->state = 0;
    printf("  -> %s 已关机\n", d->type);
    return 0;
}

static int ir_dump(device_t *d, char *buf, size_t n)
{
    snprintf(buf, n, "%-10s id=%d 状态=%s", d->type, d->id,
             d->state ? "开" : "关");
    return 0;
}

int ir_device_create(int id, const char *type,
                     unsigned char addr,
                     unsigned char cmd_on,
                     unsigned char cmd_off)
{
    if (g_ir_count >= IR_MAX)
        return -1;

    device_t  *dev  = &g_ir_dev[g_ir_count];
    ir_priv_t *priv = &g_ir_priv[g_ir_count];

    priv->addr    = addr;
    priv->cmd_on  = cmd_on;
    priv->cmd_off = cmd_off;

    dev->id   = id;
    snprintf(dev->type, sizeof dev->type, "%s", type);
    dev->state = 0;
    dev->priv  = priv;
    dev->on    = ir_on;
    dev->off   = ir_off;
    dev->dump  = ir_dump;

    g_ir_count++;
    return dev_register(dev);
}
