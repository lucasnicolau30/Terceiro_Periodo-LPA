#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int** matriz;
    int inicio;
    int fim;
    int colunas;
    unsigned int seed;
} ThreadArgs;

// Gerador de números aleatórios simples (LCG)
unsigned int lcg_rand(unsigned int *seed) {
    *seed = (1103515245 * (*seed) + 12345) & 0x7fffffff;
    return *seed;
}

void* thread_func(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    for (int i = args->inicio; i < args->fim; i++) {
        for (int j = 0; j < args->colunas; j++) {
            args->matriz[i][j] = lcg_rand(&args->seed) % 100;
        }
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <linhas> <colunas> <num_threads>\n", argv[0]);
        return 1;
    }

    int linhas = atoi(argv[1]);
    int colunas = atoi(argv[2]);
    int num_threads = atoi(argv[3]);

    printf("Executando preenchimento_matriz...\n");

    int** matriz = malloc(linhas * sizeof(int*));
    for (int i = 0; i < linhas; i++) {
        matriz[i] = malloc(colunas * sizeof(int));
    }

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    ThreadArgs* args = malloc(num_threads * sizeof(ThreadArgs));
    int parte = linhas / num_threads;

    srand((unsigned int)time(NULL));

    for (int i = 0; i < num_threads; i++) {
        args[i].matriz = matriz;
        args[i].colunas = colunas;
        args[i].inicio = i * parte;
        args[i].fim = (i == num_threads - 1) ? linhas : (i + 1) * parte;

        args[i].seed = rand(); // seed única por thread
        pthread_create(&threads[i], NULL, thread_func, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Matriz preenchida:\n");
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%2d ", matriz[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
    free(threads);
    free(args);

    return 0;
}
