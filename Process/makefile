CC=clang
CFLAGS=-Wall -Werror -Wextra -std=c11 -O3 -g
SRC=sjf.c
OBJ=sjf.o
BIN=sjf

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)
	$(CC) $(OBJ) -o $(BIN)

clear:
	rm -rf sjf.o sjf

PHONY: all clear