CC = aarch64-linux-gnu-gcc

TARGET = server
TARGET_SRCS = src/server.c src/thread.c src/handler.c

LIB_DIR = lib
LIB_NAME = librpi.so
LIB_TARGET = $(LIB_DIR)/$(LIB_NAME)
LIB_SRCS = src/buzzer.c src/cds.c src/led.c src/segment.c

.PHONY: all clean

all: $(LIB_TARGET) $(TARGET)

$(LIB_TARGET): $(LIB_SRCS)
	@mkdir -p $(LIB_DIR)
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $^ -lwiringPi -lcrypt -Iinclude

$(TARGET): $(TARGET_SRCS)
	$(CC) -o $@ $^ -ldl -rdynamic -Iinclude

clean:
	rm -f $(TARGET) $(LIB_TARGET)