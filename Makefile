CC=gcc
DEPS=compiler.h instructions.h label.h
OBJ=compiler.o instructions.o label.o
CFLAGS=-I.

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

compiler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o
	rm -rf compiler 
