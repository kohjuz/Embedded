/* menu.c - 主界面模块（应用层）
 *
 * 显示 menu.jpg（1024x600，按钮画在图上），
 * 触摸点击用坐标判断进入了哪个按钮区域。
 *
 * 按钮布局（和 menu.jpg 对应，命中区域比视觉按钮外扩 10px）：
 *   幻灯片 [50,250]x[495,585]     音乐  [254,454]x[495,585]
 *   触摸   [458,658]x[495,585]    设置  [662,862]x[495,585]
 *   音乐播放键  圆心(920,545) 半径 34   —— 全局播放/停止
 */
#include <stdio.h>

#include "app.h"
#include "libdisp.h"
#include "touch.h"
#include "music.h"

#define MENU_IMG "./menu.jpg"

/* 矩形命中测试 */
static int in_rect(int x, int y, int x1, int y1, int x2, int y2)
{
    return x >= x1 && x <= x2 && y >= y1 && y <= y2;
}

/* 圆形命中测试 */
static int in_circle(int x, int y, int cx, int cy, int r)
{
    int dx = x - cx, dy = y - cy;
    return dx * dx + dy * dy <= r * r;
}

app_mode_t menu_run(void)
{
    disp_clear(&g_fb);
    if (disp_image_file(&g_fb, MENU_IMG, 1000) != 0) {
        disp_text(&g_fb, 20, 40, "menu.jpg missing!");
        disp_text(&g_fb, 20, 70, "tap to continue");
        int tx, ty;
        touch_read(-1, &tx, &ty, NULL, NULL);
    }

    while (1) {
        int x, y, x2, y2;
        int r = touch_read(-1, &x, &y, &x2, &y2);
        /* 菜单页没有"滑动"操作：就算位移超过阈值被识别成滑动（r==2），
         * 也按起点坐标当点击处理，防止误判丢掉按钮点击 */
        if (r != 1 && r != 2)
            continue;

        /* 右下角圆形：音乐播放/停止 */
        if (in_circle(x, y, 920, 545, 34)) {
            if (music_is_playing())
                music_stop();
            else {
                int cur = music_current();
                music_play(cur >= 0 ? cur : 0);
            }
            continue;
        }

        if (in_rect(x, y, 50, 495, 250, 585))
            return MODE_SLIDE;
        if (in_rect(x, y, 254, 495, 454, 585))
            return MODE_MUSIC;
        if (in_rect(x, y, 458, 495, 658, 585))
            return MODE_PHOTO;
        if (in_rect(x, y, 662, 495, 862, 585))
            return MODE_SETTINGS;
    }
}
