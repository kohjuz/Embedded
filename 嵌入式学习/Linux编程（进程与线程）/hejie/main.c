#include <stdio.h>
#include <string.h>
#include "device.h"
#include "ir_dummy.h"

int main(void)
{
    /* 两个不同品牌、不同协议的家电，注册进来后长得一模一样 */
    ir_device_create(1, "ir_ac", 0x00, 0x45, 0x47);   /* 空调 */
    ir_device_create(2, "ir_tv", 0x01, 0x18, 0x19);   /* 电视 */

    printf("合界 v0.1 | 命令: list / on <id> / off <id> / quit\n");

    char line[64], cmd[16];
    int  id;

    while (fgets(line, sizeof line, stdin)) {
        if (sscanf(line, "%15s %d", cmd, &id) < 1)
            continue;

        if (strcmp(cmd, "quit") == 0)
            break;

        if (strcmp(cmd, "list") == 0) {
            dev_list();
            continue;
        }

        device_t *d = dev_find(id);
        if (d == NULL) {
            printf("  没有 id=%d 的设备\n", id);
            continue;
        }

        if (strcmp(cmd, "on") == 0)
            d->on(d);
        else if (strcmp(cmd, "off") == 0)
            d->off(d);
        else
            printf("  未知命令: %s\n", cmd);
    }

    printf("再见\n");
    return 0;
}
