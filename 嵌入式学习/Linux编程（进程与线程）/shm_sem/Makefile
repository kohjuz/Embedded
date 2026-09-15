CC      = gcc
CFLAGS  = -pthread -Wall
# -pthread 链接 POSIX 线程/信号量；部分发行版共享内存需 -lrt，可取消下一行注释
# LDLIBS += -lrt

all: producer consumer

producer: producer.c shm_sem.h
	$(CC) $(CFLAGS) -o producer producer.c

consumer: consumer.c shm_sem.h
	$(CC) $(CFLAGS) -o consumer consumer.c

clean:
	rm -f producer consumer

.PHONY: all clean
