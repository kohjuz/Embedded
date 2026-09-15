CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -g
TARGET  = hejie
OBJS    = main.o device.o ir_dummy.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
