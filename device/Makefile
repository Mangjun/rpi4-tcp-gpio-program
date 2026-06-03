CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -O2 -fPIC -I../server/include
LDFLAGS = -lwiringPi -lcrypt

BIN_DIR = ../bin
LIB_NAME = librpi.so
LIB_TARGET = $(BIN_DIR)/$(LIB_NAME)
LIB_SRCS = buzzer.c cds.c led.c segment.c

.PHONY: clean

$(LIB_TARGET): $(LIB_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(LIB_TARGET)