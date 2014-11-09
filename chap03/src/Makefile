CC = cc
CFLAGS = -Wall -Wextra -g -O2
OBJ = token_stack.o
BIN = cdecl

$(BIN): $(OBJ) cdecl.c
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ)

clean:
	@rm -vf *.o $(BIN)

.PHONY: clean
