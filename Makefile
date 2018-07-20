CC=gcc
CFLAGS=-I.
DEPS = ch8.h
OBJ = dissassembler.o
EMU = ch8Emulator.o
SDL = -lSDL

.phony: run
run: dissassembler
	./dissassembler TETRIS

%.0: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dissassembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

CH8Emulator: $(EMU)
	$(CC) -o $@ $^ $(CFLAGS) $(SDL)
