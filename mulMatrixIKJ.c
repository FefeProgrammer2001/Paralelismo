#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <stdatomic.h>
#include <time.h>

typedef struct {
    int n;
    int** A;
    int** B;
    int** C;
    atomic_int rows_done;
} Matrix;

typedef struct {
    Matrix* m;
    int start;
    int end;
} ThreadData;

void allocate(Matrix *m, int n) {
    m->n = n;
    atomic_init(&m->rows_done, 0);
    
    m->A = (int**)malloc(n * sizeof(int*));
    m->B = (int**)malloc(n * sizeof(int*));
    m->C = (int**)malloc(n * sizeof(int*));

    for(int i = 0; i < n; i++) {
        m->A[i] = (int*)malloc(n * sizeof(int));
        m->B[i] = (int*)malloc(n * sizeof(int));
        m->C[i] = (int*)malloc(n * sizeof(int));
    }
}

void deallocate(Matrix *m) {
    for(int i = 0; i < m->n; i++) {
        free(m->A[i]);
        free(m->B[i]);
        free(m->C[i]);
    }
    free(m->A);
    free(m->B);
    free(m->C);
}

void initialize(Matrix *m) {
    for(int i = 0; i < m->n; i++)
        for(int j = 0; j < m->n; j++) {
            m->A[i][j] = 1;
            m->B[i][j] = 1;
            m->C[i][j] = 0;
        }
}

int multiply_part(void* arg) {
    ThreadData *data = (ThreadData*)arg;
    Matrix *m = data->m;

    for(int i = data->start; i < data->end; i++) {
        for(int k = 0; k < m->n; k++) {
            int temp = m->A[i][k];
            for(int j = 0; j < m->n; j++)
                m->C[i][j] += temp * m->B[k][j];
        }
        atomic_fetch_add(&m->rows_done, 1);
    }

    return thrd_success;
}

void printMatrix(Matrix* m) {
    printf("C[0][0] = %d\n", m->C[0][0]);
    printf("Linhas processadas (atomic): %d / %d\n", atomic_load(&m->rows_done), m->n);
}

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "Uso: %s <tamanho_matriz> <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    if(n <= 0 || num_threads <= 0) {
        fprintf(stderr, "Tamanho e quantidade de threads devem ser positivos");
        return EXIT_FAILURE;
    }

    Matrix m;
    allocate(&m, n);
    initialize(&m);

    thrd_t* threads = (thrd_t*)malloc(num_threads * sizeof(thrd_t));
    ThreadData* tdata = (ThreadData*)malloc(num_threads * sizeof(ThreadData));

    int linhasPorThread = n / num_threads;
    int resto = n % num_threads;

    struct timespec ts_start, ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);

    for(int i = 0, linha = 0; i < num_threads; i++) {
        tdata[i].m = &m;
        tdata[i].start = linha;
        tdata[i].end = linha + linhasPorThread + (i < resto ? 1 : 0);
        linha = tdata[i].end;

        if(thrd_create(&threads[i], multiply_part, &tdata[i]) != thrd_success) {
            fprintf(stderr, "Erro ao criar thread %d\n", i);
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < num_threads; i++) {
        int res;
        thrd_join(threads[i], &res);
    }

    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    double elapsed = (ts_end.tv_sec - ts_start.tv_sec) + (ts_end.tv_nsec - ts_start.tv_nsec) / 1e9;
    printMatrix(&m);
    printf("Tempo: %.4f s\n", elapsed);

    free(threads);
    free(tdata);
    deallocate(&m);
    return EXIT_SUCCESS;
}