#include <exception>
#include <pthread.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <chrono>

class Matrix {
private:
    int n;
    int** A;
    int** B;
    int** C;

    struct ThreadArgs {
        Matrix* instance;
        int start;
        int end;
        int* rowsDone;
        pthread_mutex_t* mutex;
    };

    void alocar() {
        A = new int*[n];
        B = new int*[n];
        C = new int*[n];
        for(int i = 0; i < n; ++i) {
            A[i] = new int[n]();
            B[i] = new int[n]();
            C[i] = new int[n]();
        }
    }

    void desalocar() {
        for(int i = 0; i < n; ++i) {
            delete[] A[i];
            delete[] B[i];
            delete[] C[i];
        }
        delete[] A;
        delete[] B;
        delete[] C;
    }

    void multiplicarParte(int start, int end, int* rowsDone, pthread_mutex_t* mutex) {
        for(int i = start; i < end; ++i) {
            for(int j = 0; j < n; ++j) {
                int soma = 0;
                for(int k = 0; k < n; ++k)
                    soma += A[i][k] * B[k][j];
                C[i][j] = soma;
            }

            pthread_mutex_lock(mutex);
            (*rowsDone)++;
            pthread_mutex_unlock(mutex);
        }
    }

    static void* threadRoutine(void* arg) {
        ThreadArgs* args = static_cast<ThreadArgs*>(arg);
        args->instance->multiplicarParte(args->start, args->end, args->rowsDone, args->mutex);
        return nullptr;
    }

public:
    explicit Matrix(int n) : n(n), A(nullptr), B(nullptr), C(nullptr) {
        if(n <= 0) throw std::invalid_argument("Tamanho da matriz deve ser positivo");
        alocar();
    }

    ~Matrix() { desalocar(); }

    Matrix(const Matrix&) = delete;
    Matrix& operator=(const Matrix&) = delete;

    void inicializar() {
        for(int i = 0; i < n; ++i)
            for(int j = 0; j < n; ++j) {
                A[i][j] = 1;
                B[i][j] = 1;
                C[i][j] = 0;
            }
    }

    void multiplicar(int numThreads) {
        if(numThreads <= 0)
            throw std::invalid_argument("Quantidade de threads deve ser positiva");

        for(int i = 0; i < n; ++i)
            for(int j = 0; j < n; ++j)
                C[i][j] = 0;

        int rowsDone = 0;

        pthread_mutex_t mutex;
        if(pthread_mutex_init(&mutex, nullptr) != 0) {
            throw std::runtime_error("Falha ao inicializar o mutex");
        }

        pthread_t* threads = new pthread_t[numThreads];
        ThreadArgs* args = new ThreadArgs[numThreads];

        int linhasPorThread = n / numThreads;
        int resto = n % numThreads;

        for(int i = 0, linha = 0; i < numThreads; ++i) {
            int start = linha;
            int end = start + linhasPorThread + (i < resto ? 1 : 0);
            linha = end;

            args[i] = {this, start, end, &rowsDone, &mutex};

            if(pthread_create(&threads[i], nullptr, threadRoutine, &args[i]) != 0) {
                throw std::runtime_error("Falha ao criar thread");
            }
        }

        for(int i = 0; i < numThreads; ++i) {
            pthread_join(threads[i], nullptr);
        }

        pthread_mutex_destroy(&mutex);
        delete[] threads;
        delete[] args;
    }

    int getN() const { return n; }
    int getC(int i, int j) const { return C[i][j]; }

    void imprimir() const {
        std::cout << "C[0][0] = " << C[0][0] << std::endl;
    }
};

int main(int argc, char **argv) {
    if(argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <tamanho_matrix> <num_threads>\n";
        return EXIT_FAILURE;
    }

    int n = std::atoi(argv[1]);
    int numThreads = std::atoi(argv[2]);

    if(n <= 0 || numThreads <= 0) {
        std::cerr << "Tamanho e threads devem ser positivos\n";
        return EXIT_FAILURE;
    }

    try {
        Matrix m(n);
        m.inicializar();

        auto inicio = std::chrono::steady_clock::now();
        m.multiplicar(numThreads);
        auto fim = std::chrono::steady_clock::now();

        double elapsed = std::chrono::duration<double>(fim - inicio).count();
        m.imprimir();
        std::cout << "Tempo: " << std::fixed << std::setprecision(2) << elapsed << "s\n";
    } catch(const std::exception& e) {
        std::cerr << "Erro: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
