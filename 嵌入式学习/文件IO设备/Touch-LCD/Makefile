
SRC=1.c  2.c  click.c  touch.c
BIN=1  2  click touch

CC=aarch64-linux-gnu-gcc


JPEG_DIR=../作业3-1/jpeg-9d

all:$(BIN)


1 click touch: %: %.c
	$(CC) $< -o $@
	
2: 2.c jpeg_display.c
	$(CC) $^ -o $@ -I $(JPEG_DIR)/include $(JPEG_DIR)/lib/libjpeg.a

clean:
	rm -f $(BIN)

PHONY:all clean
