/*
 * picture_io.c — 图片数据获取动态库
 *
 * 功能: 封装 BMP/JPEG 解码, 对外只暴露一个 get_picture() 函数
 *       调用 get_picture("xxx.bmp/jpg", &info) 就能拿到图片数据
 *
 * 编译成动态库:
 *   gcc -shared -fPIC -o libpicture_io.so picture_io.c -ljpeg
 *   (交叉编译用 arm-linux-gnueabihf-gcc)
 *
 * 使用:
 *   1. #include "picture_io.h"
 *   2. 编译时: -L. -lpicture_io -ljpeg
 *   或用 dlopen 动态加载
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdint.h>

#include "picture_io.h"

/* ==================================================================
 *                          BMP 解码部分
 * ================================================================== */
#pragma pack(push, 1)

typedef struct {
    uint16_t signature;     /* 'BM' = 0x4D42 */
    uint32_t file_size;
    uint16_t reserved[2];
    uint32_t data_offset;   /* 像素数据起始位置 */
} __attribute__((packed)) BMPFileHeader;

typedef struct {
    uint32_t header_size;
    int32_t  width;
    int32_t  height;        /* 正=bottom-up, 负=top-down */
    uint16_t planes;
    uint16_t bpp;           /* 24 或 32 */
    uint32_t compression;
    uint32_t image_size;
    int32_t  x_ppm;
    int32_t  y_ppm;
    uint32_t colors_used;
    uint32_t colors_important;
} __attribute__((packed)) BMPInfoHeader;

#pragma pack(pop)

/* BMP 文件 -> RGB24, 成功返回0 */
static int bmp_decode(const char *file, PictureInfo *info)
{
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        perror("fopen bmp");
        return -1;
    }

    BMPFileHeader fh;
    if (fread(&fh, sizeof(fh), 1, fp) != 1) {
        fprintf(stderr, "[BMP] 读取文件头失败\n");
        fclose(fp);
        return -1;
    }
    if (fh.signature != 0x4D42) {
        fprintf(stderr, "[BMP] 不是 BMP 文件! 签名=0x%04X\n", fh.signature);
        fclose(fp);
        return -1;
    }

    BMPInfoHeader ih;
    if (fread(&ih, sizeof(ih), 1, fp) != 1) {
        fprintf(stderr, "[BMP] 读取信息头失败\n");
        fclose(fp);
        return -1;
    }

    if (ih.compression != 0) {
        fprintf(stderr, "[BMP] 不支持压缩格式\n");
        fclose(fp);
        return -1;
    }
    if (ih.bpp != 24 && ih.bpp != 32) {
        fprintf(stderr, "[BMP] 只支持 24/32 位, 当前=%d\n", ih.bpp);
        fclose(fp);
        return -1;
    }

    int w = ih.width;
    int h = ih.height > 0 ? ih.height : -ih.height;
    int is_top_down = ih.height < 0;

    /* 分配 RGB24 缓冲区 */
    char *rgb = (char *)calloc(1, (size_t)w * h * 3);
    if (!rgb) {
        fclose(fp);
        return -1;
    }

    int stride = (w * (ih.bpp / 8) + 3) & ~3;   /* 4字节对齐 */
    uint8_t *row = (uint8_t *)malloc(stride);
    if (!row) {
        free(rgb);
        fclose(fp);
        return -1;
    }

    fseek(fp, fh.data_offset, SEEK_SET);

    /* BMP 像素: 自底向上, 每像素 BGR 顺序 */
    for (int y = 0; y < h; y++) {
        if (fread(row, 1, stride, fp) != (size_t)stride) {
            fprintf(stderr, "[BMP] 读取第 %d 行失败\n", y);
            free(row); free(rgb); fclose(fp);
            return -1;
        }
        int dst_y = is_top_down ? y : (h - 1 - y);

        for (int x = 0; x < w; x++) {
            uint8_t b = row[x * (ih.bpp/8) + 0];
            uint8_t g = row[x * (ih.bpp/8) + 1];
            uint8_t r = row[x * (ih.bpp/8) + 2];
            char *p = rgb + (dst_y * w + x) * 3;
            p[0] = (char)r;
            p[1] = (char)g;
            p[2] = (char)b;
        }
    }

    free(row);
    fclose(fp);

    info->width  = w;
    info->height = h;
    info->bpp    = 24;
    info->rgb    = rgb;

    printf("[BMP] 解码成功: %s  %dx%d  %dbpp\n", file, w, h, info->bpp);
    return 0;
}

/* ==================================================================
 *                          JPEG 解码部分
 * 需要安装 libjpeg-dev:  sudo apt install libjpeg-dev
 * 如果没有 libjpeg, 用 -DNO_JPEG 编译, 动态库就只支持 BMP
 * ================================================================== */
#ifndef NO_JPEG
#include <jpeglib.h>

/* JPEG 文件 -> RGB24, 成功返回0 */
static int jpeg_decode(const char *file, PictureInfo *info)
{
    /* 1. 读取整个 jpg 文件到内存 */
    FILE *fp = fopen(file, "rb");
    if (!fp) {
        perror("fopen jpg");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long jpgsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (jpgsize <= 0) {
        fprintf(stderr, "[JPEG] 文件为空!\n");
        fclose(fp);
        return -1;
    }

    unsigned char *jpgdata = (unsigned char *)malloc(jpgsize);
    if (!jpgdata) {
        fclose(fp);
        return -1;
    }
    if (fread(jpgdata, 1, jpgsize, fp) != (size_t)jpgsize) {
        fprintf(stderr, "[JPEG] 读取文件失败\n");
        free(jpgdata);
        fclose(fp);
        return -1;
    }
    fclose(fp);

    /* 2. 用 libjpeg 解码 */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, jpgdata, (unsigned long)jpgsize);

    if (!jpeg_read_header(&cinfo, 1)) {
        fprintf(stderr, "[JPEG] 读取文件头失败\n");
        jpeg_destroy_decompress(&cinfo);
        free(jpgdata);
        return -1;
    }
    jpeg_start_decompress(&cinfo);

    int w = cinfo.output_width;
    int h = cinfo.output_height;
    int components = cinfo.output_components;   /* RGB=3 */

    char *rgb = (char *)malloc((size_t)w * h * components);
    if (!rgb) {
        jpeg_destroy_decompress(&cinfo);
        free(jpgdata);
        return -1;
    }

    unsigned long linesize = (unsigned long)w * components;
    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *buf[1];
        buf[0] = (unsigned char *)rgb + cinfo.output_scanline * linesize;
        jpeg_read_scanlines(&cinfo, buf, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(jpgdata);

    info->width  = w;
    info->height = h;
    info->bpp    = components * 8;   /* 24 */
    info->rgb    = rgb;

    printf("[JPEG] 解码成功: %s  %dx%d  %dbpp\n", file, w, h, info->bpp);
    return 0;
}
#endif /* NO_JPEG */

/* ==================================================================
 *                    对外唯一接口: get_picture
 * ==================================================================
 * 根据文件扩展名自动选择 BMP 或 JPEG 解码器
 * 调用这个函数就能拿到图片的所有数据
 */
int get_picture(const char *filename, PictureInfo *info)
{
    if (!filename || !info) {
        fprintf(stderr, "[get_picture] 参数为空!\n");
        return -1;
    }

    /* 清零 */
    memset(info, 0, sizeof(*info));

    /* 根据扩展名分派 */
    const char *ext = strrchr(filename, '.');
    if (!ext) {
        fprintf(stderr, "[get_picture] 文件无扩展名: %s\n", filename);
        return -1;
    }

    if (strcasecmp(ext, ".bmp") == 0) {
        return bmp_decode(filename, info);
    } else if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0) {
#ifdef NO_JPEG
        fprintf(stderr, "[get_picture] 本动态库未编译 JPEG 支持 (NO_JPEG)\n");
        return -1;
#else
        return jpeg_decode(filename, info);
#endif
    } else {
        fprintf(stderr, "[get_picture] 不支持的格式: %s\n", ext);
        return -1;
    }
}
