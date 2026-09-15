/* touch_photo.c - 触摸翻照片模式（应用层）
 *
 * 功能和 photo.c 的交互完全一致：
 *   进入直接显示第一张照片（无自动播放，纯手动）
 *   点左半边  = 上一张
 *   点右半边  = 下一张
 *   手指滑动  = 翻页（左滑下一张 / 右滑上一张）
 *   左上角 BACK 返回主界面
 */
#include <stdio.h>

#include "app.h"
#include "libdisp.h"
#include "touch.h"

app_mode_t touch_photo_run(void)
{
    photo_scan();                       /* 重新扫描 lib_photo */

    if (photo_count() <= 0) {           /* 目录没有图片：提示后返回 */
        disp_clear(&g_fb);
        disp_text(&g_fb, 20, 250, "no photo in lib_photo");
        disp_text(&g_fb, 20, 280, "tap to back");
        touch_read(-1, NULL, NULL, NULL, NULL);
        return MODE_MENU;
    }

    view_loop(0, 0);                    /* auto_ms=0：纯手动翻照片 */
    return MODE_MENU;                   /* 点 BACK 后回主界面 */
}
