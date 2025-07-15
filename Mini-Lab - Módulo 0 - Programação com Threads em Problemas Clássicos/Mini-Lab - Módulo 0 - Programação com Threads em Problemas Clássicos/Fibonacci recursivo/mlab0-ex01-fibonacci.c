#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int n;
    int* resultado;
} ThreadArgs;

void* fibonacci(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int n = args->n;

    if (n <= 5) {
        int a = 0, b = 1;
        if (n == 0) {
            *(args->resultado) = 0;
        } else if (n == 1) {
            *(args->resultado) = 1;
        } else {
            for (int i = 2; i <= n; i++) {
                int temp = a + b;
                a = b;
                b = temp;
            }
            *(args->resultado) = b;
        }
        return NULL;
    }

    pthread_t t1, t2;
    int* resultado1 = malloc(sizeof(int));
    int* resultado2 = malloc(sizeof(int));

    ThreadArgs* args1 = malloc(sizeof(ThreadArgs));
    args1->n = n - 1;
    args1->resultado = resultado1;

    ThreadArgs* args2 = malloc(sizeof(ThreadArgs));
    args2->n = n - 2;
    args2->resultado = resultado2;

    pthread_create(&t1, NULL, fibonacci, args1);
    pthread_create(&t2, NULL, fibonacci, args2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    *(args->resultado) = *resultado1 + *resultado2;

    free(resultado1);
    free(resultado2);
    free(args1);
    free(args2);

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <número>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int resultado;
    ThreadArgs args;
    args.n = n;
    args.resultado = &resultado;

    printf("Executando fibonacci...\n");
    pthread_t thread;
    pthread_create(&thread, NULL, fibonacci, &args);
    pthread_join(thread, NULL);

    printf("Fibonacci de %d = %d\n", n, resultado);

    return 0;
}
