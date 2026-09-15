#ifndef __IR_DUMMY_H__
#define __IR_DUMMY_H__

/* 模拟红外驱动：暂时用日志代替真实的 38kHz 载波波形。
   硬件（红外发射管 + NEC 时序）到位后，只改 ir_send() 一个函数，
   上层完全不动 —— 这就是设备抽象层存在的意义。 */
int ir_device_create(int id, const char *type,
                     unsigned char addr,
                     unsigned char cmd_on,
                     unsigned char cmd_off);
#endif
