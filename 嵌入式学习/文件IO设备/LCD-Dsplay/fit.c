#include "fit.h"

/* ============================================================
 *  fit 对齐库实现
 * ============================================================ */

/* ---------- LCD 相关 ---------- */
int lcd_open(lcd_t *lcd, const char *dev)
{
    if(lcd == NULL || dev == NULL)
        return -1;

    lcd->fd = open(dev, O_RDWR);
    if(lcd->fd == -1)
    {
        perror("open lcd failed");
        return -1;
    }

    struct fb_var_screeninfo vinfo;
    if(ioctl(lcd->fd, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        perror("ioctl vinfo failed");
        close(lcd->fd);
        return -1;
    }

    lcd->width     = vinfo.xres;
    lcd->height    = vinfo.yres;
    lcd->bpp       = vinfo.bits_per_pixel;
    lcd->line_size = lcd->width * lcd->bpp / 8;
    lcd->size      = lcd->width * lcd->height * lcd->bpp / 8;

    lcd->fb = mmap(NULL, lcd->size, PROT_READ | PROT_WRITE,
                   MAP_SHARED, lcd->fd, 0);
    if(lcd->fb == MAP_FAILED)
    {
        perror("mmap lcd failed");
        close(lcd->fd);
        return -1;
    }
    return 0;
}

void lcd_close(lcd_t *lcd)
{
    if(lcd == NULL)
        return;
    if(lcd->fb && lcd->fb != MAP_FAILED)
        munmap(lcd->fb, lcd->size);
    if(lcd->fd >= 0)
        close(lcd->fd);
    memset(lcd, 0, sizeof(*lcd));
    lcd->fd = -1;
}

/* ---------- BMP 解码 ---------- */
int bmp_load_fd(bmp_t *bmp, int fd)
{
    if(bmp == NULL || fd < 0)
        return -1;

    struct bitmap_header header;
    struct bitmap_info   info;
    bzero(&header, sizeof(header));
    bzero(&info,   sizeof(info));

    if(read(fd, &header, sizeof(header)) != sizeof(header))
    {
        perror("read bmp header failed");
        return -1;
    }
    if(read(fd, &info, sizeof(info)) != sizeof(info))
    {
        perror("read bmp info failed");
        return -1;
    }

    /* 若存在rgb_quad则跳过 */
    if(info.compression != 0)
    {
        struct rgb_quad quad;
        bzero(&quad, sizeof(quad));
        read(fd, &quad, sizeof(quad));
    }

    bmp->width  = info.width;
    bmp->height = info.height;
    bmp->bpp    = info.bit_count;
    bmp->pixel  = bmp->bpp / 8;

    /* 一行中的无效字节数 */
    int pad         = (4 - (bmp->width * bmp->bpp / 8 % 4)) % 4;
    bmp->line_size  = bmp->width * bmp->bpp / 8 + pad;
    int bmp_size    = bmp->line_size * bmp->height;

    bmp->data = malloc(bmp_size);
    if(bmp->data == NULL)
    {
        perror("malloc bmp data failed");
        return -1;
    }
    bzero(bmp->data, bmp_size);

    /* 读取RGB数据 */
    char *p = bmp->data;
    while(bmp_size > 0)
    {
        int n = read(fd, p, bmp_size);
        if(n <= 0)
        {
            perror("read bmp rgb failed");
            free(bmp->data);
            bmp->data = NULL;
            return -1;
        }
        p        += n;
        bmp_size -= n;
    }
    return 0;
}

void bmp_free(bmp_t *bmp)
{
    if(bmp == NULL)
        return;
    if(bmp->data)
        free(bmp->data);
    memset(bmp, 0, sizeof(*bmp));
}

/* ---------- 内部辅助 ---------- */

/* 计算 (x_src, y_src) 处的一个像素，直接拷贝到lcd指定偏移 */
static inline void put_pixel(const lcd_t *lcd, const bmp_t *bmp,
                             int x_lcd, int y_lcd,
                             int x_src, int y_src)
{
    if(x_lcd < 0 || x_lcd >= lcd->width ||
       y_lcd < 0 || y_lcd >= lcd->height)
        return;

    /* BMP数据是倒序存储的，最后一行在最前 */
    int bmp_row    = bmp->height - 1 - y_src;
    int bmp_offset = bmp->line_size * bmp_row + x_src * bmp->pixel;
    int lcd_offset = lcd->line_size * y_lcd   + x_lcd * 4;

    int copy_bytes = bmp->pixel < 4 ? bmp->pixel : 4;
    memcpy(lcd->fb + lcd_offset, bmp->data + bmp_offset, copy_bytes);
    /* 若bmp色深不足4字节，剩余字节保留显存原值（或补0） */
    if(copy_bytes < 4)
        memset(lcd->fb + lcd_offset + copy_bytes, 0, 4 - copy_bytes);
}

/* ---------- 各模式实现 ---------- */

static void draw_top_left(const lcd_t *lcd, const bmp_t *bmp)
{
    for(int j = 0; j < bmp->height && j < lcd->height; j++)
        for(int i = 0; i < bmp->width && i < lcd->width; i++)
            put_pixel(lcd, bmp, i, j, i, j);
}

static void draw_center(const lcd_t *lcd, const bmp_t *bmp)
{
    int off_x = (lcd->width  - bmp->width)  / 2;
    int off_y = (lcd->height - bmp->height) / 2;

    for(int j = 0; j < bmp->height; j++)
    {
        int y_lcd = off_y + j;
        if(y_lcd < 0 || y_lcd >= lcd->height) continue;
        for(int i = 0; i < bmp->width; i++)
        {
            int x_lcd = off_x + i;
            if(x_lcd < 0 || x_lcd >= lcd->width) continue;
            put_pixel(lcd, bmp, x_lcd, y_lcd, i, j);
        }
    }
}

static void draw_stretch(const lcd_t *lcd, const bmp_t *bmp)
{
    /* 拉伸铺满：LCD每个像素反查BMP对应位置 */
    for(int y = 0; y < lcd->height; y++)
    {
        int y_src = y * bmp->height / lcd->height;
        for(int x = 0; x < lcd->width; x++)
        {
            int x_src = x * bmp->width / lcd->width;
            put_pixel(lcd, bmp, x, y, x_src, y_src);
        }
    }
}

static void draw_contain(const lcd_t *lcd, const bmp_t *bmp)
{
    /* 等比缩放完整显示，可能有黑边 */
    double sx = (double)lcd->width  / bmp->width;
    double sy = (double)lcd->height / bmp->height;
    double s  = sx < sy ? sx : sy;

    int new_w = (int)(bmp->width  * s);
    int new_h = (int)(bmp->height * s);
    int off_x = (lcd->width  - new_w) / 2;
    int off_y = (lcd->height - new_h) / 2;

    for(int y = 0; y < new_h; y++)
    {
        int y_src = y * bmp->height / new_h;
        int y_lcd = off_y + y;
        for(int x = 0; x < new_w; x++)
        {
            int x_src = x * bmp->width / new_w;
            int x_lcd = off_x + x;
            put_pixel(lcd, bmp, x_lcd, y_lcd, x_src, y_src);
        }
    }
}

static void draw_cover(const lcd_t *lcd, const bmp_t *bmp)
{
    /* 等比缩放铺满，可能裁剪 */
    double sx = (double)lcd->width  / bmp->width;
    double sy = (double)lcd->height / bmp->height;
    double s  = sx > sy ? sx : sy;

    int new_w = (int)(bmp->width  * s);
    int new_h = (int)(bmp->height * s);
    int off_x = (lcd->width  - new_w) / 2;
    int off_y = (lcd->height - new_h) / 2;

    for(int y = 0; y < lcd->height; y++)
    {
        int y_src = (y - off_y) * bmp->height / new_h;
        if(y_src < 0)            y_src = 0;
        if(y_src >= bmp->height) y_src = bmp->height - 1;
        for(int x = 0; x < lcd->width; x++)
        {
            int x_src = (x - off_x) * bmp->width / new_w;
            if(x_src < 0)           x_src = 0;
            if(x_src >= bmp->width) x_src = bmp->width - 1;
            put_pixel(lcd, bmp, x, y, x_src, y_src);
        }
    }
}

/* ---------- 显示接口 ---------- */
int fit_draw(const lcd_t *lcd, const bmp_t *bmp, fit_mode_t mode)
{
    if(lcd == NULL || bmp == NULL || bmp->data == NULL)
        return -1;

    switch(mode)
    {
        case FIT_TOP_LEFT: draw_top_left(lcd, bmp); break;
        case FIT_CENTER:   draw_center  (lcd, bmp); break;
        case FIT_STRETCH:  draw_stretch (lcd, bmp); break;
        case FIT_CONTAIN:  draw_contain (lcd, bmp); break;
        case FIT_COVER:    draw_cover   (lcd, bmp); break;
        default:           draw_top_left(lcd, bmp); break;
    }
    return 0;
}

int fit_show_file(const char *bmp_path, fit_mode_t mode)
{
    if(bmp_path == NULL)
        return -1;

    lcd_t lcd;
    if(lcd_open(&lcd, "/dev/fb0") != 0)
        return -1;

    int fd = open(bmp_path, O_RDONLY);
    if(fd == -1)
    {
        perror("open bmp failed");
        lcd_close(&lcd);
        return -1;
    }

    bmp_t bmp;
    bzero(&bmp, sizeof(bmp));
    if(bmp_load_fd(&bmp, fd) != 0)
    {
        close(fd);
        lcd_close(&lcd);
        return -1;
    }
    close(fd);

    printf("图片分辨率: %d×%d\n", bmp.width, bmp.height);
    fit_draw(&lcd, &bmp, mode);

    bmp_free(&bmp);
    lcd_close(&lcd);
    return 0;
}

/* ============================================================
 *  扩展功能实现
 * ============================================================ */

/* 在LCD指定坐标(x,y)处开始绘制bmp（原始大小，超出裁剪）
 * 用法: fit_draw_at(&lcd, &bmp, 100, 50);
 * 原理: 遍历bmp每个像素，映射到LCD的(x+i, y+j)位置
 *       超出屏幕范围的像素自动跳过 */
int fit_draw_at(const lcd_t *lcd, const bmp_t *bmp, int x, int y)
{
    if(lcd == NULL || bmp == NULL || bmp->data == NULL)
        return -1;

    for(int j = 0; j < bmp->height; j++)
    {
        int y_lcd = y + j;
        if(y_lcd < 0 || y_lcd >= lcd->height) continue;  // 越界裁剪
        for(int i = 0; i < bmp->width; i++)
        {
            int x_lcd = x + i;
            if(x_lcd < 0 || x_lcd >= lcd->width) continue;  // 越界裁剪
            put_pixel(lcd, bmp, x_lcd, y_lcd, i, j);
        }
    }
    return 0;
}

/* 将bmp缩放ratio倍后绘制到LCD（居中）
 * 用法: fit_draw_scale(&lcd, &bmp, 0.5);   // 缩小一半
 *       fit_draw_scale(&lcd, &bmp, 2.0);   // 放大两倍
 * 原理: 计算缩放后尺寸 new_w/new_h，居中放置
 *       LCD上每个像素反查bmp源坐标: x_src = x * bmp_w / new_w
 *       实现最近邻插值（速度快，无平滑） */
int fit_draw_scale(const lcd_t *lcd, const bmp_t *bmp, double ratio)
{
    if(lcd == NULL || bmp == NULL || bmp->data == NULL || ratio <= 0.0)
        return -1;

    int new_w = (int)(bmp->width  * ratio);
    int new_h = (int)(bmp->height * ratio);
    if(new_w <= 0 || new_h <= 0)
        return -1;

    // 居中偏移
    int off_x = (lcd->width  - new_w) / 2;
    int off_y = (lcd->height - new_h) / 2;

    // 遍历缩放后图像，每个LCD像素反查bmp源像素
    for(int y = 0; y < new_h; y++)
    {
        int y_src = y * bmp->height / new_h;  // 反查y源坐标
        int y_lcd = off_y + y;
        if(y_lcd < 0 || y_lcd >= lcd->height) continue;
        for(int x = 0; x < new_w; x++)
        {
            int x_src = x * bmp->width / new_w;   // 反查x源坐标
            int x_lcd = off_x + x;
            if(x_lcd < 0 || x_lcd >= lcd->width) continue;
            put_pixel(lcd, bmp, x_lcd, y_lcd, x_src, y_src);
        }
    }
    return 0;
}

/* 左右镜像翻转后绘制到LCD（可配合mode指定缩放策略）
 * 用法: fit_draw_mirror(&lcd, &bmp, FIT_CENTER);   // 原始大小居中镜像
 *       fit_draw_mirror(&lcd, &bmp, FIT_STRETCH);   // 镜像后拉伸铺满
 * 原理: 先按mode算出显示区域大小和偏移（与正常绘制一致）
 *       取色时将x坐标翻转: x_src = bmp_w - 1 - x_src */
int fit_draw_mirror(const lcd_t *lcd, const bmp_t *bmp, fit_mode_t mode)
{
    if(lcd == NULL || bmp == NULL || bmp->data == NULL)
        return -1;

    // 先按mode确定显示区域起点和大小
    int new_w = bmp->width;
    int new_h = bmp->height;
    int off_x, off_y;

    switch(mode)
    {
        case FIT_STRETCH:
        case FIT_COVER:
        case FIT_CONTAIN:
        {
            // 缩放类模式：先计算缩放后尺寸
            double sx, sy, s;
            if(mode == FIT_STRETCH)
            {
                new_w = lcd->width;
                new_h = lcd->height;
            }
            else if(mode == FIT_CONTAIN)
            {
                sx = (double)lcd->width  / bmp->width;
                sy = (double)lcd->height / bmp->height;
                s  = sx < sy ? sx : sy;  // 取较小比例，完整显示
                new_w = (int)(bmp->width  * s);
                new_h = (int)(bmp->height * s);
            }
            else /* FIT_COVER */
            {
                sx = (double)lcd->width  / bmp->width;
                sy = (double)lcd->height / bmp->height;
                s  = sx > sy ? sx : sy;  // 取较大比例，铺满裁剪
                new_w = (int)(bmp->width  * s);
                new_h = (int)(bmp->height * s);
            }
            off_x = (lcd->width  - new_w) / 2;
            off_y = (lcd->height - new_h) / 2;
            break;
        }
        case FIT_CENTER:
            off_x = (lcd->width  - bmp->width)  / 2;
            off_y = (lcd->height - bmp->height) / 2;
            new_w = bmp->width;
            new_h = bmp->height;
            break;
        case FIT_TOP_LEFT:
        default:
            off_x = 0;
            off_y = 0;
            new_w = bmp->width;
            new_h = bmp->height;
            break;
    }

    // 遍历LCD显示区域，x坐标翻转取bmp像素
    for(int j = 0; j < new_h; j++)
    {
        int y_lcd = off_y + j;
        if(y_lcd < 0 || y_lcd >= lcd->height) continue;
        // 缩放模式下需要反查y源坐标
        int y_src = (mode == FIT_STRETCH || mode == FIT_CONTAIN || mode == FIT_COVER)
                    ? j * bmp->height / new_h : j;
        if(y_src < 0)            y_src = 0;
        if(y_src >= bmp->height) y_src = bmp->height - 1;

        for(int i = 0; i < new_w; i++)
        {
            int x_lcd = off_x + i;
            if(x_lcd < 0 || x_lcd >= lcd->width) continue;
            int x_src = (mode == FIT_STRETCH || mode == FIT_CONTAIN || mode == FIT_COVER)
                        ? i * bmp->width / new_w : i;
            if(x_src < 0)           x_src = 0;
            if(x_src >= bmp->width) x_src = bmp->width - 1;
            // 镜像：x翻转，左边像素取右边源数据
            x_src = bmp->width - 1 - x_src;
            put_pixel(lcd, bmp, x_lcd, y_lcd, x_src, y_src);
        }
    }
    return 0;
}

/* 旋转指定角度后绘制到LCD（居中）
 * angle 仅支持 0 / 90 / 180 / 270
 * 用法: fit_draw_rotate(&lcd, &bmp, 90);    // 顺时针90度
 *       fit_draw_rotate(&lcd, &bmp, 180);   // 180度(上下颠倒+左右翻转)
 *       fit_draw_rotate(&lcd, &bmp, 270);   // 顺时针270度(=逆时针90度)
 * 原理: 0度=直接居中绘制
 *       90度: 旋转后宽高互换, src(bmp_w-1-j, i)
 *       180度: src(bmp_w-1-i, bmp_h-1-j)
 *       270度: 旋转后宽高互换, src(j, bmp_h-1-i) */
int fit_draw_rotate(const lcd_t *lcd, const bmp_t *bmp, int angle)
{
    if(lcd == NULL || bmp == NULL || bmp->data == NULL)
        return -1;

    // 规范化角度到 0/90/180/270
    angle = ((angle % 360) + 360) % 360;
    if(angle % 90 != 0)
        return -1;  // 不支持非90度倍数

    if(angle == 0)
    {
        // 0度=直接居中绘制
        int off_x = (lcd->width  - bmp->width)  / 2;
        int off_y = (lcd->height - bmp->height) / 2;
        return fit_draw_at(lcd, bmp, off_x, off_y);
    }

    // 旋转后宽高互换（90/270度时）
    int rw, rh;
    if(angle == 90 || angle == 270)
    {
        rw = bmp->height;  // 旋转后宽=原图高
        rh = bmp->width;   // 旋转后高=原图宽
    }
    else /* 180 */
    {
        rw = bmp->width;
        rh = bmp->height;
    }

    // 居中放置
    int off_x = (lcd->width  - rw) / 2;
    int off_y = (lcd->height - rh) / 2;

    // 遍历旋转后图像，根据角度计算源坐标
    for(int j = 0; j < rh; j++)
    {
        int y_lcd = off_y + j;
        if(y_lcd < 0 || y_lcd >= lcd->height) continue;
        for(int i = 0; i < rw; i++)
        {
            int x_lcd = off_x + i;
            if(x_lcd < 0 || x_lcd >= lcd->width) continue;

            int x_src, y_src;
            switch(angle)
            {
                case 90:
                    // 顺时针90度: (i,j) <- src(bmp_w-1-j, i)
                    x_src = bmp->width  - 1 - j;
                    y_src = i;
                    break;
                case 180:
                    // 180度: (i,j) <- src(bmp_w-1-i, bmp_h-1-j)
                    x_src = bmp->width  - 1 - i;
                    y_src = bmp->height - 1 - j;
                    break;
                case 270:
                    // 顺时针270度: (i,j) <- src(j, bmp_h-1-i)
                    x_src = j;
                    y_src = bmp->height - 1 - i;
                    break;
                default:
                    x_src = i;
                    y_src = j;
                    break;
            }
            put_pixel(lcd, bmp, x_lcd, y_lcd, x_src, y_src);
        }
    }
    return 0;
}

/* 用RGB颜色清屏（整个LCD填充指定颜色）
 * 用法: fit_clear(&lcd, 0, 0, 0);         // 清成黑色
 *       fit_clear(&lcd, 255, 255, 255);    // 清成白色
 *       fit_clear(&lcd, 255, 0, 0);       // 清成红色
 * 原理: 根据色深(32/16位)将颜色写入整个显存
 *       32位: 直接写 0x00RRGGBB
 *       16位: 转换为RGB565格式 */
void fit_clear(const lcd_t *lcd, unsigned char r,
                                 unsigned char g,
                                 unsigned char b)
{
    if(lcd == NULL || lcd->fb == NULL)
        return;

    // 组合颜色值: ARGB格式(32位) 或 RGB565(16位)
    unsigned int color = (r << 16) | (g << 8) | b;
    if(lcd->bpp == 32)
    {
        unsigned int *p32 = (unsigned int *)lcd->fb;
        for(int i = 0; i < lcd->width * lcd->height; i++)
            p32[i] = color;
    }
    else if(lcd->bpp == 16)
    {
        // RGB565: r取高5位, g取高6位, b取高5位
        unsigned short c565 = ((r >> 3) << 11) |
                              ((g >> 2) << 5)  |
                              (b >> 3);
        unsigned short *p16 = (unsigned short *)lcd->fb;
        for(int i = 0; i < lcd->width * lcd->height; i++)
            p16[i] = c565;
    }
}

/* 用RGB颜色填充LCD上的矩形区域
 * 用法: fit_fill_rect(&lcd, 0, 0, 100, 50, 255, 0, 0);    // 左上角100x50红色块
 *       fit_fill_rect(&lcd, 50, 50, 200, 100, 0, 255, 0); // 绿色矩形
 * 参数: (x,y)矩形左上角, w/h宽高, r/g/b颜色(0~255)
 * 原理: 先将矩形裁剪到屏幕范围内，再逐行填充颜色 */
void fit_fill_rect(const lcd_t *lcd, int x, int y, int w, int h,
                   unsigned char r, unsigned char g, unsigned char b)
{
    if(lcd == NULL || lcd->fb == NULL)
        return;

    // 裁剪到屏幕范围内（处理负坐标和越界）
    if(x < 0) { w += x; x = 0; }
    if(y < 0) { h += y; y = 0; }
    if(x + w > lcd->width)  w = lcd->width  - x;
    if(y + h > lcd->height) h = lcd->height - y;
    if(w <= 0 || h <= 0) return;

    unsigned int color = (r << 16) | (g << 8) | b;
    if(lcd->bpp == 32)
    {
        // 32位色深: 逐行填充
        for(int j = 0; j < h; j++)
        {
            unsigned int *row = (unsigned int *)(lcd->fb +
                             lcd->line_size * (y + j));
            for(int i = 0; i < w; i++)
                row[x + i] = color;
        }
    }
    else if(lcd->bpp == 16)
    {
        // 16位色深: 转RGB565后逐行填充
        unsigned short c565 = ((r >> 3) << 11) |
                              ((g >> 2) << 5)  |
                              (b >> 3);
        for(int j = 0; j < h; j++)
        {
            unsigned short *row = (unsigned short *)(lcd->fb +
                              lcd->line_size * (y + j));
            for(int i = 0; i < w; i++)
                row[x + i] = c565;
        }
    }
}

/* 将bmp绘制到LCD上的指定矩形区域内（拉伸至该区域大小）
 * 用法: fit_draw_rect(&lcd, &bmp, 0, 0, 200, 200);    // 拉伸到左上角200x200
 *       fit_draw_rect(&lcd, &bmp, 50, 50, 300, 200);  // 拉伸到指定区域
 * 参数: (x,y)目标区域左上角, w/h目标区域宽高
 * 原理: 遍历目标区域内每个LCD像素，反查bmp源坐标
 *       x_src = (lcd_x - x) * bmp_w / w
 *       相当于FIT_STRETCH但限制在指定区域而非全屏 */
int fit_draw_rect(const lcd_t *lcd, const bmp_t *bmp,
                  int x, int y, int w, int h)
{
    if(lcd == NULL || bmp == NULL || bmp->data == NULL)
        return -1;
    if(w <= 0 || h <= 0)
        return -1;

    // 裁剪目标矩形到屏幕范围
    int dx0 = x, dy0 = y;
    int dx1 = x + w, dy1 = y + h;
    if(dx0 < 0) dx0 = 0;
    if(dy0 < 0) dy0 = 0;
    if(dx1 > lcd->width)  dx1 = lcd->width;
    if(dy1 > lcd->height) dy1 = lcd->height;

    // 遍历目标区域，每个LCD像素反查bmp源坐标
    for(int j = dy0; j < dy1; j++)
    {
        // 反查y源坐标: (lcd_y - 目标y偏移) * bmp_h / 目标h
        int y_src = (j - y) * bmp->height / h;
        if(y_src < 0)            y_src = 0;
        if(y_src >= bmp->height) y_src = bmp->height - 1;

        for(int i = dx0; i < dx1; i++)
        {
            // 反查x源坐标
            int x_src = (i - x) * bmp->width / w;
            if(x_src < 0)           x_src = 0;
            if(x_src >= bmp->width) x_src = bmp->width - 1;
            put_pixel(lcd, bmp, i, j, x_src, y_src);
        }
    }
    return 0;
}
