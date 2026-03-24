#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int n;
    int** A;
    int** B;
    int** C;
} Matrix;

typedef struct {
    Matrix* m;
    int start;
    int end;
} ThreadData;

void allocate(Matrix* m, int n) {
    m->n = n;
    m->A = (int**)calloc(n, sizeof(int*));
    m->B = (int**)calloc(n, sizeof(int*));
    m->C = (int**)calloc(n, sizeof(int*));
    for (int i = 0; i < n; i++) {
        m->A[i] = (int*)calloc(n, sizeof(int));
        m->B[i] = (int*)calloc(n, sizeof(int));
        m->C[i] = (int*)calloc(n, sizeof(int));
    }
}

void deallocate(Matrix* m) {
    for (int i = 0; i < m->n; i++) {
        free(m->A[i]);
        free(m->B[i]);
        free(m->C[i]);
    }
    free(m->A);
    free(m->B);
    free(m->C);
}

void initialize(Matrix* m) {
    for (int i = 0; i < m->n; i++) {
        for (int j = 0; j < m->n; j++) {
            m->A[i][j] = 1;
            m->B[i][j] = 1;
            m->C[i][j] = 0;
        }
    }
}

void* multiply_part(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    Matrix* m = data->m;

    for (int i = data->start; i < data->end; i++) {
        for (int j = 0; j < m->n; j++) {
            for (int k = 0; k < m->n; k++) {
                m->C[i][j] += m->A[i][k] * m->B[k][j];
            }
        }
    }
    pthread_exit(NULL);
}

void printMatrix(Matrix* m) {
    printf("C[0][0] = %d\n", m->C[0][0]);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <matrix_size> <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int numThreads = atoi(argv[2]);

    if (n <= 0 || numThreads <= 0) {
        fprintf(stderr, "Matrix size must be a positive value.\n");
        return EXIT_FAILURE;
    }

    Matrix m;
    allocate(&m, n);
    initialize(&m);

    pthread_t* threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    ThreadData* tdata = (ThreadData*)malloc(numThreads * sizeof(ThreadData));

    int linhasPorThread = n / numThreads;
    int resto = n % numThreads;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0, linha = 0; i < numThreads; ++i) {
        tdata[i].m = &m;
        tdata[i].start = linha;
        tdata[i].end = linha + linhasPorThread + (i < resto ? 1 : 0);
        linha = tdata[i].end;

        pthread_create(&threads[i], NULL, multiply_part, &tdata[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printMatrix(&m);
    printf("Tempo: %.4f\n", elapsed);

    free(threads);
    free(tdata);
    deallocate(&m);
    return EXIT_SUCCESS;
}
