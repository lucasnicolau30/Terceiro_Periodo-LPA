#include <pthread.h>
#include <stdio.h>
#include <time.h>  // Para usar clock()

#define N_THREADS 8
#define N_ITERS 1000000

int contador = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* tarefa(void* arg) {
    for (int i = 0; i < N_ITERS; i++) {
        pthread_mutex_lock(&lock);  // Bloqueia o mutex
        contador++;
        pthread_mutex_unlock(&lock);  // Desbloqueia o mutex
    }
    return NULL;
}

int main() {
    pthread_t threads[N_THREADS];

    // Começa a medir o tempo
    clock_t start = clock();

    // Cria as threads
    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, tarefa, NULL);
    }

    // Espera as threads terminarem
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Para de medir o tempo
    clock_t end = clock();

    // Calcula o tempo de execução
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // Exibe o resultado
    printf("Contador final: %d (esperado: %d)\n", contador, N_THREADS * N_ITERS);
    printf("Tempo de execução com mutex: %.6f segundos\n", time_spent);

    pthread_mutex_destroy(&lock);
    return 0;
}