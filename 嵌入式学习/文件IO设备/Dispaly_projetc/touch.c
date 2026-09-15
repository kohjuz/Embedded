/* touch.c - 触摸屏封装（库层）
 *
 * 原理：
 *   触摸屏是输入设备，数据从 /dev/input/eventX 读入，
 *   每个事件是一个 struct input_event。
 *   - ABS_X / ABS_Y : 触点坐标
 *   - BTN_TOUCH     : 按下(1)/抬起(0)
 *
 * 设计：
 *   poll() 支持超时，让"自动播放"这类定时功能不用死等触摸。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <linux/input.h>

#include "touch.h"

static int g_fd = -1;

/* 手势状态必须跨调用保留！
 * 内核触摸事件是分批发来的（按下包 / 抬起包），
 * 如果 sx/sy/cx/cy/pressed 是局部变量，一次"按下→抬起"
 * 被拆成两次 touch_read 调用后，抬起会被当成孤儿事件丢掉。
 * （photo.c 的 click() 在单次调用里阻塞读完整个手势，所以没事）
 */
static int sx, sy, cx, cy;   /* 手势起点 / 当前坐标 */
static int pressed = 0;      /* 是否处于按下状态 */
static int anchored = 0;     /* 起点是否已锚定 */
static int got_x = 0, got_y = 0;  /* 本批次内是否收到过 X/Y 坐标 */

/* 滑动阈值：起止点距离小于 40 视为"点击"，否则视为"滑动"
 * 20 太小：电容屏手指按下时轻微抖动就超过 20px，
 * 会被误判成"滑动"丢掉，导致按钮要点好几次才生效 */

/* 为什么不能信 BTN_TOUCH=1 瞬间的 cx/cy？
 * 内核事件流按"批次"发送（SYN_REPORT 分隔），多数驱动顺序是：
 *   BTN_TOUCH 1  ->  ABS_X ->  ABS_Y ->  SYN_REPORT
 * 按下事件到达时 cx/cy 还是上一次手势残留的坐标（首次是 0,0），
 * 直接拿它当起点，位移会算成几百像素 -> 误判滑动 -> 点击被丢。
 * 正确做法：等按下后"同一批次"内收到第一对 ABS_X/ABS_Y 再锚定起点。*/
#define SWIPE_THRESHOLD  40

int touch_open(const char *dev)
{
    g_fd = open(dev, O_RDWR);
    if (g_fd < 0) {
        perror("touch_open");
        return -1;
    }
    return 0;
}

int touch_read(int timeout_ms, int *x, int *y, int *x2, int *y2)
{
    if (g_fd < 0)
        return -1;

    struct input_event ev;

    while (1) {
        struct pollfd pfd = { .fd = g_fd, .events = POLLIN };
        /* 手势进行中：必须等到抬起才算完，不设超时（和 photo.c 的
         * 阻塞 read 语义一致）；空闲时才按 timeout_ms 等新事件 */
        int pr = poll(&pfd, 1, pressed ? -1 : timeout_ms);
        if (pr == 0)
            return 0;               /* 空闲超时 */
        if (pr < 0)
            return -1;

        ssize_t n = read(g_fd, &ev, sizeof(ev));
        if (n < (ssize_t)sizeof(ev))
            continue;               /* 没读全，继续等 */

        if (ev.type == EV_ABS && ev.code == ABS_X) {
            cx = ev.value;
            got_x = 1;
            /* 按下状态 + 本批次 X、Y 都到了 -> 这是本次按下点，锚定起点 */
            if (pressed && !anchored && got_x && got_y) {
                sx = cx;
                sy = cy;
                anchored = 1;
            }
        }
        else if (ev.type == EV_ABS && ev.code == ABS_Y) {
            cy = ev.value;
            got_y = 1;
            if (pressed && !anchored && got_x && got_y) {
                sx = cx;
                sy = cy;
                anchored = 1;
            }
        }
        else if (ev.type == EV_KEY && ev.code == BTN_TOUCH) {
            if (ev.value == 1) {    /* 按下：等待锚定起点 */
                if (!pressed) {
                    pressed = 1;
                    anchored = 0;
                    /* 驱动若先发 ABS 再发 BTN（坐标已就绪），立即锚定 */
                    if (got_x && got_y) {
                        sx = cx;
                        sy = cy;
                        anchored = 1;
                    }
                }
            } else if (ev.value == 0) { /* 抬起：完成一次手势 */
                if (pressed) {
                    pressed = 0;
                    /* 兜底：整个手势没收到坐标，起点=终点，必判点击 */
                    if (!anchored) {
                        sx = cx;
                        sy = cy;
                    }
                    if (x)   *x  = sx;
                    if (y)   *y  = sy;
                    if (x2)  *x2 = cx;
                    if (y2)  *y2 = cy;
                    /* 距离阈值判断：点击 or 滑动 */
                    int dx = cx - sx;
                    int dy = cy - sy;
                    if (dx * dx + dy * dy <= SWIPE_THRESHOLD * SWIPE_THRESHOLD)
                        return 1;   /* 点击 */
                    return 2;       /* 滑动 */
                }
                /* 无按下就直接抬起：忽略 */
            }
        }
        else if (ev.type == EV_SYN && ev.code == SYN_REPORT) {
            /* 批次结束：清掉"本批次收到坐标"标记，
             * 让下一次手势从干净的起点开始算 */
            got_x = got_y = 0;
        }
    }
}
