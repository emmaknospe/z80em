CC=gcc
DEPS=compiler.h instructions.h label.h emulator.h
OBJ_COMP=compiler.o instructions.o label.o 
OBJ_EM=emulator.o
CFLAGS=-I.

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

compiler: $(OBJ_COMP)
	$(CC) -o $@ $^ $(CFLAGS)

emulator: $(OBJ_EM)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o
	rm compiler 
	rm emulator 
