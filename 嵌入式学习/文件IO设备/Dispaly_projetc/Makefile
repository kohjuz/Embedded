# ============================================================
# 数码相框构建脚本
#   photo  - 你原来的程序（photo.c + jpeg_display.c，保留不动）
#   frame  - 新主界面程序（menu/slide/music/touch + 库层）
#
# 库层（未来可编成 .so 动态库）: libdisp.c touch.c
# 应用层: main.c menu.c slide.c music.c touch_photo.c
# ============================================================
CC       = aarch64-linux-gnu-gcc
JPEG_DIR = $(HOME)/jpeg-9d

CFLAGS   = -I $(JPEG_DIR)/build/include -Wall
LDFLAGS  = -L $(JPEG_DIR)/build/lib
LDLIBS   = -ljpeg -lpthread

all: photo frame
	scp frame root@192.168.53.26:/root/swj

# ---- photo：你原来的程序 ----
photo: photo.o jpeg_display.o
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

photo.o: photo.c jpeg_display.h
	$(CC) $(CFLAGS) -c $< -o $@

# ---- frame：新主界面程序 ----
FRAME_OBJ = main.o menu.o slide.o music.o touch_photo.o \
            libdisp.o touch.o jpeg_display.o

frame: $(FRAME_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# 通用编译规则：.c -> .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 头文件变了，相关 .o 自动重编（简化的依赖管理）
$(FRAME_OBJ): app.h touch.h libdisp.h music.h font8x16.h

clean:
	rm -f photo frame *.o

.PHONY: all clean
