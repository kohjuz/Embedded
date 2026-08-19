/*
 * bmp2fb.c — 将 BMP 图片显示到 Linux Framebuffer (/dev/fb0)
 *
 * 编译: gcc bmp2fb.c -o bmp2fb          (不需要额外库！)
 * 运行: ./bmp2fb 1.bmp                  # 直接显示
 *       ./bmp2fb 1.bmp fit               # 等比缩放适应屏幕
 *       ./bmp2fb 1.bmp stretch           # 拉伸铺满全屏
 *
 * 支持的 BMP 格式: 24位(BGR) / 32位(BGRA)
 *
 * 原理:
 *   BMP 文件 = 54字节头 + 原始像素数据
 *   fread 读出头信息 → 得到宽高/色深
 *   fread 读出像素 → BGR 转 ARGB8888
 *   mmap /dev/fb0 → 直接 memcpy 进显存
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <stdint.h>

/* ==================== BMP 文件头结构 ====================
 *
 *  BMP 文件布局 (Little-Endian):
 *
 *  ┌──────────────┬───────────────────┬────────────────────┐
 *  │  File Header │    Info Header    │   Pixel Data       │
 *  │   (14 字节)  │    (40 字节)      │   (自底向上)        │
 *  └──────────────┴───────────────────┴────────────────────┘
 *
 *  File Header:
 *    [0-1]   'BM' 签名
 *    [2-5]   文件总大小
 *    [6-9]   保留字段(0)
 *    [10-13] 像素数据偏移量(通常=54)
 *
 *  Info Header (BITMAPINFOHEADER):
 *    [14-17] 头大小 (=40)
 *    [18-21] 图片宽度 (像素)
 *  [22-25] 图片高度 (正数= bottom-up)
 *    [26-27] 平面数 (=1)
 *  [28-29] 每像素位数 (24 或 32)
 *  [30-33] 压缩方式 (0=无压缩)
 *    ...     后面还有几字段，一般用不到
 */
#pragma pack(push, 1)

typedef struct {
    uint16_t signature;     /* 'BM' = 0x4D42 */
    uint32_t file_size;
    uint16_t reserved[2];
    uint32_t data_offset;   /* 像素数据起始位置 */
} __attribute__((packed)) BMPFileHeader;

typedef struct {
    uint32_t header_size;   /* 信息头大小 = 40 */
    int32_t  width;         /* 宽度 */
    int32_t  height;        /* 高度 (正=bottom-up) */
    uint16_t planes;        /* 必须为 1 */
    uint16_t bpp;           /* bits per pixel: 24或32 */
    uint32_t compression;   /* 0=BI_RGB 无压缩 */
    uint32_t image_size;    /* 像素数据大小(可为0) */
    int32_t  x_ppm;         /* 水平分辨率(像素/米) */
    int32_t  y_ppm;         /* 垂直分辨率 */
    uint32_t colors_used;   /* 颜色表颜色数 */
    uint32_t colors_important;
} __attribute__((packed)) BMPInfoHeader;

#pragma pack(pop)

/* ARGB8888 像素 */
typedef struct {
    uint8_t b, g, r, a;
} BGRA_Pixel;

/* Framebuffer 信息 */
typedef struct {
    int fd;
    uint32_t *fb;
    int width, height;
    int line_len;           /* 每行字节数(含padding) */
    int screen_size;
} FBInfo;

/* ==================== Framebuffer 操作 ==================== */

int fb_open(FBInfo *info)
{
    info->fd = open("/dev/fb0", O_RDWR);
    if (info->fd < 0) {
        perror("open /dev/fb0");
        return -1;
    }

    /* 获取屏幕实际参数 */
    struct fb_var_screeninfo vinfo;
    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("ioctl FBIOGET_VSCREENINFO");
        close(info->fd);
        return -1;
    }

    info->width  = vinfo.xres;
    info->height = vinfo.yres;
    info->line_len = vinfo.xres * (vinfo.bits_per_pixel / 8);
    info->screen_size = info->line_len * vinfo.yres;

    printf("[FB] 屏幕: %dx%d, %dbpp, 总计 %d 字节\n",
           info->width, info->height, vinfo.bits_per_pixel,
           info->screen_size);

    info->fb = mmap(NULL, info->screen_size,
                    PROT_WRITE, MAP_SHARED, info->fd, 0);
    if (info->fb == MAP_FAILED) {
        perror("mmap");
        close(info->fd);
        return -1;
    }

    return 0;
}

void fb_close(FBInfo *info)
{
    if (info->fb != MAP_FAILED)
        munmap(info->fb, info->screen_size);
    if (info->fd >= 0)
        close(info->fd);
}

/* 用黑色清屏 */
void fb_clear(FBInfo *info)
{
    memset(info->fb, 0, info->screen_size);
}

/* ==================== BMP 读取 ==================== */

/*
 * 读取 BMP 文件，返回解码后的 ARGB8888 像素数组
 * 调用者负责 free(*out_pixels)
 */
int bmp_load(const char *filename,
             uint32_t **out_pixels, int *out_w, int *out_h)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen bmp");
        return -1;
    }

    /* 读文件头 */
    BMPFileHeader fh;
    if (fread(&fh, sizeof(fh), 1, fp) != 1) {
        fprintf(stderr, "读取文件头失败\n");
        fclose(fp);
        return -1;
    }

    if (fh.signature != 0x4D42) {  /* 'BM' */
        fprintf(stderr, "不是 BMP 文件! 签名=0x%04X\n", fh.signature);
        fclose(fp);
        return -1;
    }

    /* 读信息头 */
    BMPInfoHeader ih;
    if (fread(&ih, sizeof(ih), 1, fp) != 1) {
        fprintf(stderr, "读取信息头失败\n");
        fclose(fp);
        return -1;
    }

    printf("[BMP] 尺寸: %dx%d, 色深: %d bpp, 压缩: %u\n",
           ih.width, ih.height, ih.bpp, ih.compression);

    /* 只支持 24位和 32位无压缩 BMP */
    if (ih.compression != 0) {
        fprintf(stderr, "不支持压缩格式! compression=%u\n", ih.compression);
        fclose(fp);
        return -1;
    }
    if (ih.bpp != 24 && ih.bpp != 32) {
        fprintf(stderr, "只支持 24/32 位 BMP! 当前=%d bpp\n", ih.bpp);
        fclose(fp);
        return -1;
    }

    int w = ih.width;
    int h = ih.height > 0 ? ih.height : -ih.height;  /* 取绝对值 */
    int is_top_down = ih.height < 0;                   /* 负数=top-down */

    /* 分配输出缓冲区 (ARGB8888) */
    uint32_t *pixels = (uint32_t *)malloc(w * h * 4);
    if (!pixels) {
        fprintf(stderr, "malloc 失败!\n");
        fclose(fp);
        return -1;
    }

    /* BMP 每行按 4 字节对齐 */
    int stride = (w * (ih.bpp / 8) + 3) & ~3;
    uint8_t *row_buf = (uint8_t *)malloc(stride);
    if (!row_buf) {
        free(pixels);
        fclose(fp);
        return -1;
    }

    /* 定位到像素数据起始位置 */
    fseek(fp, fh.data_offset, SEEK_SET);

    /*
     * BMP 像素存储顺序: 自底向上 (bottom-up)
     * 即文件中第一行对应图片最下面一行
     * 每个像素: B, G, R [, A] (蓝绿红顺序!)
     */
    for (int y = 0; y < h; y++) {
        if (fread(row_buf, 1, stride, fp) != (size_t)stride) {
            fprintf(stderr, "读取第 %d 行失败\n", y);
            free(pixels); free(row_buf); fclose(fp);
            return -1;
        }

        /* 目标行号：bottom-up 需要翻转 */
        int dst_y = is_top_down ? y : (h - 1 - y);

        for (int x = 0; x < w; x++) {
            uint8_t b = row_buf[x * (ih.bpp/8) + 0];
            uint8_t g = row_buf[x * (ih.bpp/8) + 1];
            uint8_t r = row_buf[x * (ih.bpp/8) + 2];
            /* 32位 BMP 有 alpha 通道，24位的默认不透明 */
            uint8_t a = (ih.bpp == 32) ? row_buf[x * 4 + 3] : 0xFF;

            /* 组装 ARGB8888 */
            pixels[dst_y * w + x] = ((uint32_t)a << 24) |
                                    ((uint32_t)r << 16) |
                                    ((uint32_t)g << 8)  |
                                     (uint32_t)b;
        }
    }

    free(row_buf);
    fclose(fp);

    *out_pixels = pixels;
    *out_w = w;
    *out_h = h;
    return 0;
}

/* ==================== 缩放算法 (最近邻插值) ==================== */

/*
 * 最近邻缩放 — 快但质量一般，适合嵌入式场景
 * 双线性插值更平滑但计算量大，这里用最简方案
 */
uint32_t *scale_nearest(const uint32_t *src, int sw, int sh,
                        int dw, int dh)
{
    uint32_t *dst = (uint32_t *)malloc(dw * dh * 4);
    if (!dst) return NULL;

    for (int dy = 0; dy < dh; dy++) {
        for (int dx = 0; dx < dw; dx++) {
            /* 映射回源坐标 */
            int sx = (dx * sw) / dw;
            int sy = (dy * sh) / dh;
            dst[dy * dw + dx] = src[sy * sw + sx];
        }
    }
    return dst;
}

/* 等比缩放：保持比例，居中显示，空白填黑 */
uint32_t *scale_fit(const uint32_t *src, int sw, int sh,
                    int dw, int dh, int *out_w, int *out_h)
{
    /* 计算缩放比例 */
    double scale_x = (double)dw / sw;
    double scale_y = (double)dh / sh;
    double scale = (scale_x < scale_y) ? scale_x : scale_y;  /* 取较小值 */

    int new_w = (int)(sw * scale);
    int new_h = (int)(sh * scale);

    printf("[缩放] 等比 %.2fx: %dx%d -> %dx%d (居中于 %dx%d)\n",
           scale, sw, sh, new_w, new_h, dw, dh);

    uint32_t *scaled = scale_nearest(src, sw, sh, new_w, new_h);
    if (!scaled) return NULL;

    /* 创建带黑边的画布 */
    uint32_t *canvas = (uint32_t *)calloc(dw * dh, 4);
    if (!canvas) { free(scaled); return NULL; }

    /* 居中偏移 */
    int offset_x = (dw - new_w) / 2;
    int offset_y = (dh - new_h) / 2;

    for (int y = 0; y < new_h; y++) {
        for (int x = 0; x < new_w; x++) {
            canvas[(y + offset_y) * dw + (x + offset_x)]
                = scaled[y * new_w + x];
        }
    }

    free(scaled);
    *out_w = dw;
    *out_h = dh;
    return canvas;
}

/* ==================== 主函数 ==================== */

void print_usage(const char *prog)
{
    printf("用法:\n"
           "  %s <图片.bmp>              原尺寸显示(超出部分裁剪)\n"
           "  %s <图片.bmp> fit           等比缩放+居中(推荐)\n"
           "  %s <图片.bmp> stretch       拉伸铺满全屏\n",
           prog, prog, prog);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *bmp_file = argv[1];
    const char *mode = (argc >= 3) ? argv[2] : "raw";  /* raw/fit/stretch */

    /* 1. 读取 BMP */
    uint32_t *pixels = NULL;
    int img_w, img_h;

    if (bmp_load(bmp_file, &pixels, &img_w, &img_h) != 0)
        return 1;

    printf("[BMP] 加载成功: %dx%d (%.1f KB)\n",
           img_w, img_h, (float)(img_w * img_h * 4) / 1024);

    /* 2. 打开 Framebuffer */
    FBInfo fb;
    if (fb_open(&fb) != 0) {
        free(pixels);
        return 1;
    }

    /* 3. 清屏 */
    fb_clear(&fb);

    /* 4. 根据模式处理图像 */
    uint32_t *display = pixels;
    int disp_w = img_w, disp_h = img_h;
    int need_free = 0;  /* 是否需要 free display */

    if (strcmp(mode, "stretch") == 0) {
        /* 拉伸铺满 */
        display = scale_nearest(pixels, img_w, img_h, fb.width, fb.height);
        disp_w = fb.width;
        disp_h = fb.height;
        need_free = 1;
        printf("[显示] 拉伸模式: -> %dx%d\n", disp_w, disp_h);

    } else if (strcmp(mode, "fit") == 0) {
        /* 等比缩放居中 */
        display = scale_fit(pixels, img_w, img_h,
                            fb.width, fb.height, &disp_w, &disp_h);
        need_free = 1;

    } else {
        /* raw 模式：原尺寸，左上角对齐，超出的裁剪 */
        printf("[显示] 原尺寸: %dx%d (左上角对齐)\n", img_w, img_h);
    }

    if (!display) {
        fprintf(stderr, "缩放失败!\n");
        fb_close(&fb);
        free(pixels);
        return 1;
    }

    /* 5. 写入 framebuffer */
    int copy_w = (disp_w < fb.width)  ? disp_w : fb.width;
    int copy_h = (disp_h < fb.height) ? disp_h : fb.height;

    for (int y = 0; y < copy_h; y++) {
        /* 逐行 memcpy，处理行可能不对齐的情况 */
        memcpy(fb.fb + y * fb.width,
               display + y * disp_w,
               copy_w * 4);
    }

    printf("[完成] 已写入 %d x %d 像素到 /dev/fb0\n", copy_w, copy_h);

    /* 6. 清理 */
    if (need_free && display) free(display);
    free(pixels);
    fb_close(&fb);

    return 0;
}
