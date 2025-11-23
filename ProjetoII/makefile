# Compilador e flags comuns
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11
OMPFLAGS = -fopenmp

# Alvos (executáveis)
TARGETS = analyzer_seq analyzer_par_critical analyzer_par_atomic

# Regra padrão: compila tudo
all: $(TARGETS)

# ---- VERSÃO SEQUENCIAL ----
analyzer_seq: analyzer_seq.o hash_table.o
	$(CC) $(CFLAGS) $^ -o $@

# ---- VERSÃO PARALELA (critical) ----
analyzer_par_critical: analyzer_par_critical.o hash_table.o
	$(CC) $(CFLAGS) $(OMPFLAGS) $^ -o $@

# ---- VERSÃO PARALELA (atomic) ----
analyzer_par_atomic: analyzer_par_atomic.o hash_table.o
	$(CC) $(CFLAGS) $(OMPFLAGS) $^ -o $@

# Regra genérica para compilar .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar objetos e binários
clean:
	rm -f *.o $(TARGETS) results.csv

# Limpar tudo (se quiser garantir que não sobra nada de execuções anteriores)
distclean: clean
	rm -f *~ core
