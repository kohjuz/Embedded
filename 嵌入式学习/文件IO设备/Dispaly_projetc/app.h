/* app.h - 应用层公共定义（模式枚举 + 各模式入口） */
#ifndef __APP_H__
#define __APP_H__

#include "jpeg_display.h"

/* ========== 模式状态机 ========== */
/* 注意：不能叫 mode_t！POSIX 标准里 mode_t 已存在(文件权限用) */
typedef enum {
    MODE_MENU,        /* 主界面 */
    MODE_SLIDE,       /* 幻灯片（自动播放） */
    MODE_MUSIC,       /* 音乐 */
    MODE_PHOTO,       /* 触摸翻照片（photo.c 风格，手动） */
    MODE_SETTINGS,    /* 设置/关于 */
    MODE_EXIT         /* 退出 */
} app_mode_t;

/* 全局屏幕（main.c 里定义，各模式共用） */
extern FBInfo g_fb;

/* 各模式入口：运行该模式，返回下一个要进入的模式 */
app_mode_t menu_run(void);
app_mode_t slide_run(void);
app_mode_t music_run(void);
app_mode_t touch_photo_run(void);
app_mode_t settings_run(void);

/* 图片浏览共享接口（slide.c 实现，slide / touch_photo 共用） */
void photo_scan(void);                  /* 扫描 lib_photo */
int  photo_count(void);                 /* 图片数量 */
int  view_loop(int idx, int auto_ms);   /* 大图浏览页：auto_ms>0 自动播放 */

#endif
