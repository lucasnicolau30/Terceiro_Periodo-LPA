#include <pthread.h>
#include <stdio.h>
#include <time.h>  // Para usar clock()

#define N_THREADS 8
#define N_ITERS 1000000

int contador = 0;
pthread_spinlock_t splock;

void* tarefa(void* arg) {
    for (int i = 0; i < N_ITERS; i++) {
        pthread_spin_lock(&splock);
        contador++;
        pthread_spin_unlock(&splock);
    }
    return NULL;
}

int main() {
    pthread_t threads[N_THREADS];

    // Inicializa o spinlock
    pthread_spin_init(&splock, PTHREAD_PROCESS_PRIVATE);

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
    printf("Tempo de execução com spinlock: %.6f segundos\n", time_spent);

    pthread_spin_destroy(&splock);
    return 0;
}