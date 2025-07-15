#include "fila_prioridade.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N_PROD 2
#define N_CONS 2
#define N_REPR 1

void* produtor(void* arg) {
    while (1) {
        Pedido p;
        p.id_dispositivo = rand() % 50;
        p.timestamp = time(NULL);
        p.prioridade = rand() % 100;
        inserir(p);
        usleep(100000); // 100 ms
    }
    return NULL;
}

void* consumidor(void* arg) {
    while (1) {
        Pedido p = remover();
        if (p.id_dispositivo != -1) {
            printf("Consumidor processou pedido do dispositivo %d com prioridade %d\n", p.id_dispositivo, p.prioridade);
        }
        usleep(150000); // 150 ms
    }
    return NULL;
}

void* atualizador(void* arg) {
    while (1) {
        int id = rand() % 50;
        int nova_prioridade = rand() % 100;
        int resultado = repriorizar(id, nova_prioridade);
        if (resultado) {
            printf("Repriorizado dispositivo %d para prioridade %d\n", id, nova_prioridade);
        }
        usleep(200000); // 200 ms
    }
    return NULL;
}

int main() {
    pthread_t prod[N_PROD], cons[N_CONS], repr[N_REPR];
    inicializar_fila();
    srand(time(NULL));

    for (int i = 0; i < N_PROD; i++)
        pthread_create(&prod[i], NULL, produtor, NULL);
    for (int i = 0; i < N_CONS; i++)
        pthread_create(&cons[i], NULL, consumidor, NULL);
    for (int i = 0; i < N_REPR; i++)
        pthread_create(&repr[i], NULL, atualizador, NULL);

    for (int i = 0; i < N_PROD; i++)
        pthread_join(prod[i], NULL);
    for (int i = 0; i < N_CONS; i++)
        pthread_join(cons[i], NULL);
    for (int i = 0; i < N_REPR; i++)
        pthread_join(repr[i], NULL);

    return 0;
}
