/*
 * picture_szie_io.c — 测试程序: 调用 libpicture_io.so 获取图片数据
 *
 * 演示如何使用动态库:
 *   方式1: 编译时链接  (-lpicture_io)
 *   方式2: 运行时加载  (dlopen)
 *
 * 本程序用方式2 (dlopen), 这样换图片格式连程序都不用重新编译
 *
 * 编译:
 *   gcc -o picture_szie_io picture_szie_io.c -ldl
 *
 * 运行:
 *   ./picture_szie_io 1.bmp
 *   ./picture_szie_io 2.jpg
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "picture_io.h"

/* 动态库中 get_picture 函数的类型 */
typedef int (*get_picture_func_t)(const char *, PictureInfo *);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("用法: %s <图片文件>\n", argv[0]);
        printf("示例: %s 1.bmp\n", argv[0]);
        printf("      %s 2.jpg\n", argv[0]);
        return 1;
    }

    const char *img_file = argv[1];

    /* ========== 1. 动态加载 libpicture_io.so ========== */
    void *handle = dlopen("./libpicture_io.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "dlopen 失败: %s\n", dlerror());
        return 1;
    }

    /* 获取 get_picture 函数指针 */
    get_picture_func_t get_picture =
        (get_picture_func_t)dlsym(handle, "get_picture");
    if (!get_picture) {
        fprintf(stderr, "dlsym 失败: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    printf("=== 动态库加载成功, 开始获取图片数据 ===\n\n");

    /* ========== 2. 调用动态库获取图片数据 ========== */
    PictureInfo info;
    if (get_picture(img_file, &info) != 0) {
        fprintf(stderr, "获取图片数据失败!\n");
        dlclose(handle);
        return 1;
    }

    /* ========== 3. 此时图片数据就在 info 里了! ========== */
    printf("\n");
    printf("======================================\n");
    printf("  图片数据获取成功!\n");
    printf("======================================\n");
    printf("  文件名 : %s\n", img_file);
    printf("  宽度   : %d 像素\n", info.width);
    printf("  高度   : %d 像素\n", info.height);
    printf("  色深   : %d bpp\n", info.bpp);
    printf("  数据量 : %d 字节\n", info.width * info.height * (info.bpp / 8));
    printf("  RGB指针: %p\n", (void *)info.rgb);
    printf("======================================\n\n");

    /* 打印前 10 个像素的 RGB 值, 证明数据确实拿到了 */
    printf("前10个像素的RGB值:\n");
    int count = 10;
    if (count > info.width * info.height) count = info.width * info.height;
    for (int i = 0; i < count; i++) {
        unsigned char r = info.rgb[i * 3 + 0];
        unsigned char g = info.rgb[i * 3 + 1];
        unsigned char b = info.rgb[i * 3 + 2];
        printf("  像素[%2d]: R=%3d G=%3d B=%3d\n", i, r, g, b);
    }

    /* ========== 4. 释放资源 ========== */
    free(info.rgb);      /* 用完一定要 free! */
    dlclose(handle);

    printf("\n[完成] 程序结束\n");
    return 0;
}
