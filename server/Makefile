CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -O2 -Iinclude
LDFLAGS = -rdynamic -pthread -ldl

LIB_TARGET = lib/librpi.so
TARGET = server
TARGET_SRCS = src/server.c src/thread.c src/handler.c src/list.c

.PHONY: clean

$(TARGET): $(TARGET_SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(LIB_TARGET)