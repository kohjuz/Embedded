# Makefile — io_lib 图片相关动态库合集
#
# 三个动态库:
#   libpicture_io.so    图片解码库   (BMP/JPG 文件 → RGB24 数据)   [原有]
#   liblcd_io.so        LCD显示库    (打开屏幕/清屏/RGB24上屏)      [新增]
#   libpicture_show.so  图片显示库   (一行代码显示 BMP/JPG 图片)     [新增]
#
# 程序里显示一张图, 编译时加上两个库:
#   gcc main.c -L. -lpicture_show -llcd_io -lpicture_io \
#       -Wl,-rpath,'$ORIGIN' -o main
# 然后一行调用:
#   #include "picture_show.h"
#   photo_display("1.jpg");   // 自动判断 BMP/JPG
#
# 用法:
#   make              编译全部 (3个库 + 测试 + 演示程序)
#   make lib          只编译3个动态库
#   make test         编译解码测试程序 (dlopen 方式)
#   make demo         编译图片显示演示程序
#   make clean        清理
#   make run_demo     编译并运行演示 (板子上有 /dev/fb0 才能看到画面)

CC     = gcc
CFLAGS = -Wall -O2 -fPIC

# 自动检测系统有没有 libjpeg (只有解码库用)
# 有 → 支持 BMP+JPG; 没有 → 只支持 BMP (-DNO_JPEG)
HAS_JPEG := $(shell find /usr/include /usr/local/include -name "jpeglib.h" 2>/dev/null | head -1)
ifneq ($(HAS_JPEG),)
    JPEG_LIB  = -ljpeg
    JPEG_FLAG =
    $(info >> 检测到 libjpeg, 解码库支持 BMP + JPEG)
else
    JPEG_LIB  =
    JPEG_FLAG = -DNO_JPEG
    $(info >> 未检测到 libjpeg, 解码库只支持 BMP)
    $(info >> (装 JPEG 支持: sudo apt install libjpeg-dev))
endif

# 三个动态库
LIB_IO  = libpicture_io.so     # 解码库
LIB_LCD = liblcd_io.so         # LCD 显示库
LIB_PIC = libpicture_show.so   # 图片显示库

# 测试 / 演示程序
TEST_BIN = picture_szie_io
DEMO_BIN = show_demo

# 让可执行文件运行时从自己所在目录找依赖的动态库
RPATH = -Wl,-rpath,'$$ORIGIN'

# 用户程序链接图片显示库需要的库 (写进说明)
USER_LIBS = -L. -lpicture_show -llcd_io -lpicture_io

.PHONY: all lib test demo clean run_demo

all: lib test demo

# ========== 1. 编译三个动态库 ==========
lib: $(LIB_IO) $(LIB_LCD) $(LIB_PIC)

$(LIB_IO): picture_io.c picture_io.h
	$(CC) $(CFLAGS) $(JPEG_FLAG) -shared -o $@ picture_io.c $(JPEG_LIB)

$(LIB_LCD): lcd_io.c lcd_io.h
	$(CC) $(CFLAGS) -shared -o $@ lcd_io.c

$(LIB_PIC): picture_show.c picture_show.h lcd_io.h picture_io.h \
            $(LIB_LCD) $(LIB_IO)
	$(CC) $(CFLAGS) -shared -o $@ picture_show.c \
	    -L. -llcd_io -lpicture_io $(RPATH)

# ========== 2. 编译测试 / 演示程序 ==========
test: $(TEST_BIN)

$(TEST_BIN): picture_szie_io.c picture_io.h
	$(CC) $(CFLAGS) -o $@ picture_szie_io.c -ldl

demo: $(DEMO_BIN)

$(DEMO_BIN): show_demo.c picture_show.h $(LIB_PIC)
	$(CC) $(CFLAGS) -o $@ show_demo.c $(USER_LIBS) $(RPATH)

# ========== 3. 清理 / 运行 ==========
clean:
	rm -f $(LIB_IO) $(LIB_LCD) $(LIB_PIC) $(TEST_BIN) $(DEMO_BIN)

run_demo: all
	./$(DEMO_BIN) wall_b.jpg
