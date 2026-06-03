CC = gcc
CFLAGS = -Wall -O2 -Iinclude

BIN_DIR = ../bin
TARGET = $(BIN_DIR)/client
SRCS = src/main.c src/menu.c src/setup.c

.PHONY: clean

$(TARGET): $(SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)