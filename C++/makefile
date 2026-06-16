CC=clang++
CFLAGS=-Wall -Werror -Wextra -std=c++23 -g
SRC1=Programa.cpp
SRC2=Otimizado.cpp
BIN1=Programa
BIN2=Otimizado

all: $(BIN1) $(BIN2)

$(BIN1): $(SRC1)
	$(CC) $(CFLAGS) $(SRC1) -o $(BIN1) -lpthread

$(BIN2): $(SRC2)
	$(CC) $(CFLAGS) $(SRC2) -o $(BIN2) -lpthread

clear:
	rm Programa Otimizado

.PHONY: clear
