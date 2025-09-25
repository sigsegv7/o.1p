CFILES = $(shell find src/ -name "*.c")
OBJ = $(CFILES:.c=.o)
CFLAGS = -Isrc/include/ -pedantic -fPIC
OUTPUT = libonet.so
CC = gcc

$(OUTPUT): $(OBJ)
	$(CC) -shared -o $@ $(OBJ)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: install
install:
	mkdir -p /usr/include/onet/
	cp -r src/include/* /usr/include/onet/
	cp libonet.so /usr/lib/
	chmod 0755 /usr/lib/libonet.so
	ldconfig

.PHONY: clean
clean:
	rm -f $(OBJ)
