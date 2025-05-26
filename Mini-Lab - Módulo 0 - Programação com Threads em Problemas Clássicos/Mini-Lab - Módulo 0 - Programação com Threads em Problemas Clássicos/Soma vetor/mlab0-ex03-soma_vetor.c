#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct{
    int* vetor;
    int inicio;
    int fim;
    int* resultado;
} ThreadArgs;

void* thread_func(void* arg){
    ThreadArgs* args = (ThreadArgs*)arg;
    
    int soma = 0;
    for (int i = args->inicio; i < args->fim; i++){
        soma += args->vetor[i];
    }
    *(args->resultado) = soma;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    int tamanho = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    printf("Executando soma_vetor...\n");
    int* vetor = malloc(tamanho * sizeof(int));
    for (int i = 0; i < tamanho; i++){
        vetor[i] = rand() % 100;
    }

    printf("Vetor gerado:\n");
    for (int i = 0; i < tamanho; i++){
        printf("%d ", vetor[i]);
    }
    printf("\n");

    // Determinar o tamanho de cada parte do vetor para cada thread
    int parte = tamanho / num_threads;

    // Criar as threads e armazenar os resultados parciais
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    ThreadArgs* args = malloc(num_threads * sizeof(ThreadArgs));
    int* resultadosParciais = malloc(num_threads * sizeof(int));

    // Criar as threads
    for (int i = 0; i < num_threads; i++){
        args[i].vetor = vetor;
        args[i].inicio = i * parte;
        if (i == num_threads - 1) {
            args[i].fim = tamanho;
        } 
        else {
            args[i].fim = (i + 1) * parte;
        }
        args[i].resultado = &resultadosParciais[i];
        pthread_create(&threads[i], NULL, thread_func, &args[i]);
    }

    for (int i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
    }

    int soma_total = 0;
    for (int i = 0; i < num_threads; i++){
        soma_total += resultadosParciais[i];
    }

    printf("Soma total: %d\n", soma_total);

    free(vetor);
    free(threads);
    free(args);
    free(resultadosParciais);

    return 0;
}