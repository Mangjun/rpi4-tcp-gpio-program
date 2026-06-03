CC = gcc
CFLAGS = -Wall -O2 -Iinclude
TARGET = client

SRCS = src/main.c src/menu.c src/setup.c

.PHONY: clean

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)