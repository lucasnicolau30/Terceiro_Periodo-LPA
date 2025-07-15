#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int* vetor;
    int inicio;
    int fim;
    int valorBuscado;
    int* resultado;
} ThreadArgs;

void* thread_func(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int inicio = args->inicio;
    int fim = args->fim;
    int valor = args->valorBuscado;
    int* vetor = args->vetor;

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        if (vetor[meio] == valor) {
            *(args->resultado) = meio;
            return NULL;
        } else if (vetor[meio] < valor) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    *(args->resultado) = -1;
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <valor_a_buscar>\n", argv[0]);
        return 1;
    }

    int valor = atoi(argv[1]);

    printf("Executando busca_binaria...\n");
    int tamanhoVetor = 100;
    int* vetor = malloc(tamanhoVetor * sizeof(int));
    for (int i = 0; i < tamanhoVetor; i++) {
        vetor[i] = i;
    }

    pthread_t t1, t2;
    int* resultado1 = malloc(sizeof(int));
    int* resultado2 = malloc(sizeof(int));

    ThreadArgs* args1 = malloc(sizeof(ThreadArgs));
    args1->vetor = vetor;
    args1->inicio = 0;
    args1->fim = tamanhoVetor / 2 - 1;
    args1->valorBuscado = valor;
    args1->resultado = resultado1;

    ThreadArgs* args2 = malloc(sizeof(ThreadArgs));
    args2->vetor = vetor;
    args2->inicio = tamanhoVetor / 2;
    args2->fim = tamanhoVetor - 1;
    args2->valorBuscado = valor;
    args2->resultado = resultado2;

    pthread_create(&t1, NULL, thread_func, args1);
    pthread_create(&t2, NULL, thread_func, args2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    if (*resultado1 != -1) {
        printf("Valor %d estava na posicao %d e foi encontrado pela thread 1.\n", valor, *resultado1);
    } else if (*resultado2 != -1) {
        printf("Valor %d estava na posicao %d e foi encontrado pela thread 2.\n", valor, *resultado2);
    } else {
        printf("Valor %d nao foi encontrado.\n", valor);
    }

    free(vetor);
    free(resultado1);
    free(resultado2);
    free(args1);
    free(args2);

    return 0;
}
