/* main.c - 数码相框入口（应用层）
 *
 * 主循环就是一个状态机：
 *   MODE_MENU -> 各模式 -> 返回 MODE_MENU -> ...
 *
 * 整个程序只在这里初始化一次屏幕和触摸，
 * 每个模式是一个函数，互不干扰，方便后续拆动态库。
 */
#include <stdio.h>

#include "app.h"
#include "touch.h"
#include "libdisp.h"
#include "music.h"

FBInfo g_fb;                /* 全局屏幕 */

/* ========== 设置/关于页 ========== */
app_mode_t settings_run(void)
{
    while (1) {
        disp_clear(&g_fb);
        disp_text(&g_fb, 20, 14, "About");
        char line[128];

        snprintf(line, sizeof(line), "Screen: %dx%d  %dbpp",
                 g_fb.width, g_fb.height, g_fb.bpp);
        disp_text(&g_fb, 20, 60, line);

        snprintf(line, sizeof(line), "Touch : /dev/input/event6");
        disp_text(&g_fb, 20, 100, line);

        snprintf(line, sizeof(line), "Photo : ./lib_photo");
        disp_text(&g_fb, 20, 140, line);

        snprintf(line, sizeof(line), "Music : ./music_lib  (wav)");
        disp_text(&g_fb, 20, 180, line);

        disp_text(&g_fb, 20, 260, "BACK: tap top-left / swipe");
        disp_text(&g_fb, 20, 290, "Version: 0.1 (basic)");

        int x, y, x2, y2;
        int r = touch_read(-1, &x, &y, &x2, &y2);
        if (r == 2)
            return MODE_MENU;               /* 滑动返回 */
        if (r == 1 && x < 130 && y < 60)
            return MODE_MENU;
    }
}

int main(void)
{
    if (touch_open("/dev/input/event6") < 0)
        return -1;
    if (fb_open(&g_fb) < 0)
        return -1;

    music_init();                           /* 扫描音乐列表 */

    app_mode_t mode = MODE_MENU;
    while (mode != MODE_EXIT) {
        switch (mode) {
        case MODE_MENU:       mode = menu_run();       break;
        case MODE_SLIDE:      mode = slide_run();      break;
        case MODE_MUSIC:      mode = music_run();      break;
        case MODE_PHOTO:      mode = touch_photo_run(); break;
        case MODE_SETTINGS:   mode = settings_run();   break;
        default:              mode = MODE_EXIT;
        }
    }

    music_stop();
    fb_close(&g_fb);
    printf("[frame] bye\n");
    return 0;
}
