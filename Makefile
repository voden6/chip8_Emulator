CC=gcc
CFLAGS=-I.
DEPS = ch8.h
OBJ = dissassembler.o

.phony: run
run: dissassembler
	./dissassembler TETRIS

%.0: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dissassembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


