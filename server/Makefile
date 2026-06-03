CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -O2 -Iinclude
LDFLAGS = -rdynamic -pthread -ldl -lwiringPi -lcrypt

BIN_DIR = ../bin
TARGET = $(BIN_DIR)/server
TARGET_SRCS = src/server.c src/thread.c src/handler.c src/list.c

.PHONY: clean

$(TARGET): $(TARGET_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)