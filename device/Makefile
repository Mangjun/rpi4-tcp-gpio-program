CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -O2 -fPIC
LDFLAGS = -lwiringPi -lcrypt

LIB_DIR = ../server/lib
LIB_NAME = librpi.so
LIB_TARGET = $(LIB_DIR)/$(LIB_NAME)
LIB_SRCS = buzzer.c cds.c led.c segment.c

.PHONY: clean

$(LIB_TARGET): $(LIB_SRCS)
	@mkdir -p $(LIB_DIR)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(LIB_TARGET)