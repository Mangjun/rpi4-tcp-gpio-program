.PHONY: all clean client device server

all: client device server

client:
	$(MAKE) -C client

device:
	$(MAKE) -C device

server:
	$(MAKE) -C server

clean:
	$(MAKE) -C client clean
	$(MAKE) -C device clean
	$(MAKE) -C server clean