CFILES = $(shell find src/ -name "*.c")
CFLAGS = -Isrc/include/ -pedantic
OUTPUT = bin/otrx
CC = gcc

.PHONY: all
all: bin
	$(CC) $(CFILES) $(CFLAGS) -o $(OUTPUT)

.PHONY: bin
bin:
	mkdir -p $@
