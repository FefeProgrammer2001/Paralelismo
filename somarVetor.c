#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdatomic.h>
#include <threads.h>

#define N (1UL << 30)

typedef struct {
    size_t* vetor;
    size_t n;
} Vetor;

typedef struct {
    Vetor* v;
    size_t start;
    size_t end;
    atomic_long* resultado;
} ThreadData;

void alocar(Vetor* v, size_t n) {
    v->n = n;
    v->vetor = (size_t*)malloc(n * sizeof(size_t));
    if(!v->vetor) {
        fprintf(stderr, "Erro ao alocar vetor\n");
        exit(EXIT_FAILURE);
    }
}

void inicializar(Vetor* v) {
    for(size_t i = 0; i < v->n; i++) {
        v->vetor[i] = 1;
    }
}

int somar_parcial(void* arg) {
    ThreadData *data = (ThreadData*)arg;
    long soma_local = 0;

    for(size_t i = data->start; i < data->end; i++)
        soma_local += data->v->vetor[i];

    atomic_fetch_add(data->resultado, soma_local);

    return thrd_success;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Uso: %s <num_threads>\n", argv[0]);
        return -1;
    }

    int num_threads = atoi(argv[1]);
    if(num_threads <= 0) {
        fprintf(stderr, "Quantidade de threads deve ser positivo\n");
        return -1;
    }

    printf("Somando os valores do vetor...\n");
    printf("Tamanho do vetor: %lu elementos\n", (unsigned long)N);
    printf("Threads utilizadas: %d\n\n", num_threads);

    Vetor v;
    alocar(&v, N);
    inicializar(&v);

    atomic_long resultado;
    atomic_init(&resultado, 0);

    thrd_t* threads = (thrd_t*)malloc(num_threads * sizeof(thrd_t));
    ThreadData* tdata = (ThreadData*)malloc(num_threads * sizeof(ThreadData));

    if(!threads || !tdata) {
        fprintf(stderr, "Falha ao alocar threads\n");
        return -1;
    }

    size_t elementosPorThread = N / num_threads;
    size_t resto = N % num_threads;

    struct timespec ts_start, ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);

    for(int i = 0, pos = 0; i < num_threads; i++) {
        tdata[i].v = &v;
        tdata[i].start = pos;
        tdata[i].end = pos + elementosPorThread + (i < (int)resto ? 1 : 0);
        tdata[i].resultado = &resultado;
        pos = tdata[i].end;

        if(thrd_create(&threads[i], somar_parcial, &tdata[i]) != thrd_success) {
            fprintf(stderr, "Erro ao criar thread %d\n", i);
            return -1;
        }
    }

    for(int i = 0; i < num_threads; i++) {
        int res;
        thrd_join(threads[i], &res);
    }

    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    double elapsed = (ts_end.tv_sec - ts_start.tv_sec) + (ts_end.tv_nsec - ts_start.tv_nsec) / 1e9;

    printf("Resultado: %ld\n", atomic_load(&resultado));
    printf("Tempo para executar: %.4fs\n", elapsed);

    free(threads);
    free(tdata);
    free(v.vetor);
    return 0;
}
