#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int* vetor;
    int inicio;
    int fim;
    int* resultado;
} ThreadArgs;

void* thread_func(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    int soma = 0;
    for (int i = args->inicio; i < args->fim; i++) {
        soma += args->vetor[i];
    }

    *(args->resultado) = soma;
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamanho_vetor> <numero_threads>\n", argv[0]);
        return 1;
    }

    int tamanho = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    printf("Executando soma_vetor...\n");

    int* vetor = malloc(tamanho * sizeof(int));
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = rand() % 100;
    }

    printf("Vetor gerado:\n");
    for (int i = 0; i < tamanho; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

    int parte = tamanho / num_threads;

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    ThreadArgs* args = malloc(num_threads * sizeof(ThreadArgs));
    int* resultadosParciais = malloc(num_threads * sizeof(int));

    for (int i = 0; i < num_threads; i++) {
        args[i].vetor = vetor;
        args[i].inicio = i * parte;
        args[i].fim = (i == num_threads - 1) ? tamanho : (i + 1) * parte;
        args[i].resultado = &resultadosParciais[i];

        pthread_create(&threads[i], NULL, thread_func, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    int soma_total = 0;
    for (int i = 0; i < num_threads; i++) {
        soma_total += resultadosParciais[i];
    }

    printf("Soma total: %d\n", soma_total);

    free(vetor);
    free(threads);
    free(args);
    free(resultadosParciais);

    return 0;
}
