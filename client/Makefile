CC = gcc
CFLAGS = -Wall -Iinclude
TARGET = client

SRCS = src/main.c src/menu.c src/setup.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGET)