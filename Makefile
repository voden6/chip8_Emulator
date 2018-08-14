CC=gcc
CFLAGS=-I.
LIBS = -lSDL2

VPATH = src

all: CH8Emulator CH8Dissassembler

CH8Emulator: ch8Emulator.c ch8.h
	$(CC) -o CH8Emulator ./src/ch8Emulator.c $(LIBS) -g -I./src

CH8Dissassembler: dissassembler.c
	$(CC) -o CH8Dissassembler ./src/dissassembler.c -I./src

.PHONY: clean

clean:
	rm CH8Emulator CH8Dissassembler
