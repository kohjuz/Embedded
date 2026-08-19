#include <stdio.h>
#include <string.h>     // memset
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <math.h>

#define W       1024
#define H       600
#define PI      3.14159265358979323846

/* 国旗标准色 ARGB8888 */
#define FLAG_RED    0xFFDE2910   /* 国旗红 #DE2910 */
#define STAR_YELLOW 0xFFFFDE00   /* 星黄 #FFDE00 */

/* ============ 五角星数学模型 ============
 * 五角星有 10 个顶点：5 个外顶点 + 5 个内顶点（凹点）
 * 外顶点半径 R，内顶点半径 r ≈ R * sin(18°) / sin(54°) ≈ R * 0.382
 * 
 *        ①外          ⑤外
 *         /\          /\
 *    ②内/  \③内   ①内/  \⑤内
 *      /    \        /    \
 *  ③外------★------⑤外   (★=圆心)
 *      \    /        \    /
 *    ④内\  /①内   ③内\  /④内
 *         \/          \/
 *        ④外          ②外
 *
 * 角度规律：外顶点从 -90°（正上）开始每隔 72° 一个
 *          内顶点从 -90°+36° 开始每隔 72° 一个
 * ============================================ */

typedef struct {
    int x, y;
} Point;

/* 计算五角星的 10 个顶点（返回堆数组，调用者负责使用）
 * cx,cy: 中心坐标
 * R:     外接圆半径
 * angle: 旋转角度（弧度），正值为顺时针。用于让小星的一角指向大星
 */
void calc_star_points(int cx, int cy, int R, double angle_rad, Point pts[10])
{
    double r_inner = R * 0.381966;  /* 2*sin(18°)/ (sin(36°)+sin(72°)) 的近似 */
    for (int i = 0; i < 5; i++) {
        /* 外顶点: -90° + i*72° + 旋转 */
        double a_out = angle_rad + (-PI/2.0 + i * 2.0 * PI / 5.0);
        pts[i * 2].x = cx + (int)(R * cos(a_out));
        pts[i * 2].y = cy + (int)(R * sin(a_out));
        
        /* 内顶点: -90°+36° + i*72° + 旋转 */
        double a_in  = angle_rad + (-PI/2.0 + PI/5.0 + i * 2.0 * PI / 5.0);
        pts[i * 2 + 1].x = cx + (int)(r_inner * cos(a_in));
        pts[i * 2 + 1].y = cy + (int)(r_inner * sin(a_in));
    }
}

/* 扫描线填充多边形（通用，支持任意凸/凹多边形）
 * 算法：对每条扫描线 y，求与所有边的交点 x，排序后两两配对填充
 */
void fill_polygon(unsigned int *fb, Point pts[], int n_pts, unsigned int color)
{
    int min_y = pts[0].y, max_y = pts[0].y;
    for (int i = 1; i < n_pts; i++) {
        if (pts[i].y < min_y) min_y = pts[i].y;
        if (pts[i].y > max_y) max_y = pts[i].y;
    }
    
    /* 裁剪到屏幕范围 */
    if (min_y < 0) min_y = 0;
    if (max_y >= H) max_y = H - 1;
    
    for (int y = min_y; y <= max_y; y++) {
        int intersections[20];  /* 最多10边→20个交点 */
        int n_inter = 0;
        
        for (int i = 0; i < n_pts; i++) {
            int j = (i + 1) % n_pts;
            int y1 = pts[i].y, y2 = pts[j].y;
            int x1 = pts[i].x, x2 = pts[j].x;
            
            /* 检查这条边是否跨越当前扫描线 */
            if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) {
                /* 计算交点 x 坐标（线性插值） */
                int inter_x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                if (n_inter < 20)
                    intersections[n_inter++] = inter_x;
            }
        }
        
        /* 对交点排序（简单冒泡，数量少够用） */
        for (int i = 0; i < n_inter - 1; i++)
            for (int j = i + 1; j < n_inter; j++)
                if (intersections[j] < intersections[i]) {
                    int tmp = intersections[i];
                    intersections[i] = intersections[j];
                    intersections[j] = tmp;
                }
        
        /* 两两配对填充水平段 */
        for (int i = 0; i + 1 < n_inter; i += 2) {
            int x_start = intersections[i];
            int x_end   = intersections[i + 1];
            if (x_end < 0 || x_start >= W) continue;
            if (x_start < 0) x_start = 0;
            if (x_end >= W) x_end = W - 1;
            
            for (int x = x_start; x <= x_end; x++)
                fb[y * W + x] = color;
        }
    }
}

/* 画一颗五角星的便捷函数 */
void draw_star(unsigned int *fb, int cx, int cy, int R, 
               double rotate_angle, unsigned int color)
{
    Point pts[10];
    calc_star_points(cx, cy, R, rotate_angle, pts);
    fill_polygon(fb, pts, 10, color);
}

/* 计算"让小星一角指向大星中心"所需的旋转角度
 * 小星默认第一顶点在正上方(-90°)，需要旋转使其指向 (target_x, target_y)
 * 返回值：弧度
 */
double calc_rotation_to(int star_cx, int star_cy, int target_x, int target_y)
{
    return atan2(target_y - star_cy, target_x - star_cx) + PI / 2.0;
}

int main(void)
{
    int fd = open("/dev/fb0", O_RDWR);
    if (fd < 0) { perror("open /dev/fb0"); return -1; }

    /* 获取实际屏幕信息（可选，但推荐） */
    struct fb_var_screeninfo vinfo;
    ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
    printf("fb: %dx%d, bpp=%d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    int size = W * H * 4;
    unsigned int *fb = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (fb == MAP_FAILED) { perror("mmap"); close(fd); return -1; }

    /* ===== 优化1: memset 批量填红色背景 ===== */
    memset(fb, 0x10, size);  /* 低字节重复 → 接近红色 */

    /* 但要精确颜色还是得逐像素（memset 只能填单字节重复模式）*/
    /* 折中方案：用 uint32_t 快速循环 */
    for (int i = 0; i < W * H; i++)
        fb[i] = FLAG_RED;

    /* ===== 国旗标准比例 (GB 12982-2004) =====
     * 旗面分为 4×4 格：
     * - 大星中心在左 1/4、上 1/4 位置偏左上一点
     * - 大星外接圆直径 = 旗面高 × 3/10
     * - 小星外接圆直径 = 旗面高 × 1/10
     * - 四颗小星在大星右侧拱形分布
     *
     * 1024×600 下的坐标计算：
     * 大星半径 R_big  = 600 * 3/10 / 2 = 90
     * 小星半径 R_small = 600 * 1/10 / 2 = 30
     * 大星中心 ≈ (156, 150)
     */

    int big_x   = W * 1 / 6;       /* ~170 */
    int big_y   = H * 1 / 4;       /* ~150 */
    int R_big   = H * 3 / 20;      /* 90 */
    int R_small = H * 1 / 20;      /* 30 */

    /* 画大五角星（不旋转，一个角朝上） */
    draw_star(fb, big_x, big_y, R_big, 0, STAR_YELLOW);

    /* 四颗小星位置（相对大星中心的偏移，按国旗标准排列） */
    struct { int x, y; } small_pos[4] = {
        { big_x + R_big + R_small + 40, big_y - R_big + 20 },   /* 右上 */
        { big_x + R_big + R_small + 80, big_y - 10 },             /* 右中上 */
        { big_x + R_big + R_small + 80, big_y + R_big - 30 },    /* 右中下 */
        { big_x + R_big + R_small + 40, big_y + R_big + 10 },    /* 右下 */
    };

    /* 画四颗小五角星，各有一角指向大星中心 */
    for (int i = 0; i < 4; i++) {
        double angle = calc_rotation_to(small_pos[i].x, small_pos[i].y,
                                        big_x, big_y);
        draw_star(fb, small_pos[i].x, small_pos[i].y, R_small, 
                  angle, STAR_YELLOW);
    }

    munmap(fb, size);
    close(fd);

    printf("中国国旗绘制完成! 1024x600 ARGB8888\n");
    return 0;
}
