CC=gcc
DEPS_RAW=compiler.h instructions.h label.h emulator.h
OBJ_COMP_RAW=compiler.o instructions.o label.o 
OBJ_EM_RAW=emulator.o
CFLAGS=-I.
SRC=./src
BIN=./bin

DEPS=$(addprefix $(SRC)/, $(DEPS_RAW))
OBJ_COMP=$(addprefix $(BIN)/, $(OBJ_COMP_RAW))
OBJ_EM=$(addprefix $(BIN)/, $(OBJ_EM_RAW))

all: compiler emulator 

$(BIN)/%.o: $(SRC)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

compiler: $(OBJ_COMP)
	$(CC) -o $@ $^ $(CFLAGS)

emulator: $(OBJ_EM)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean


clean:
	rm -rf $(BIN)
	mkdir $(BIN)
