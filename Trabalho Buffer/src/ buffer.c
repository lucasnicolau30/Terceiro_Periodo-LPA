#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TAM_BUFFER 5
#define NUM_PRODUTORES 3
#define NUM_CONSUMIDORES 2

int buffer[TAM_BUFFER];
int inicio = 0, fim = 0, count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_prod = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_cons = PTHREAD_COND_INITIALIZER;

void inserir(int item, int id) {
    pthread_mutex_lock(&mutex);

    while (count == TAM_BUFFER)
        pthread_cond_wait(&cond_prod, &mutex);

    buffer[fim] = item;
    fim = (fim + 1) % TAM_BUFFER;
    count++;

    printf("[Produtor %d] Inseriu: %d\n", id, item);

    pthread_cond_signal(&cond_cons);
    pthread_mutex_unlock(&mutex);
}

int remover(int id) {
    pthread_mutex_lock(&mutex);

    while (count == 0)
        pthread_cond_wait(&cond_cons, &mutex);

    int item = buffer[inicio];
    inicio = (inicio + 1) % TAM_BUFFER;
    count--;

    printf("[Consumidor %d] Removeu: %d\n", id, item);

    pthread_cond_signal(&cond_prod);
    pthread_mutex_unlock(&mutex);

    return item;
}

void* produtor(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        int item = rand() % 100;
        inserir(item, id);
        sleep(3);
    }
    free(arg);
    return NULL;
}

void* consumidor(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 15; i++) {
        remover(id);
        sleep(3);
    }
    free(arg);
    return NULL;
}

int main() {
    pthread_t produtores[NUM_PRODUTORES], consumidores[NUM_CONSUMIDORES];

    for (int i = 0; i < NUM_PRODUTORES; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&produtores[i], NULL, produtor, id);
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&consumidores[i], NULL, consumidor, id);
    }

    for (int i = 0; i < NUM_PRODUTORES; i++)
        pthread_join(produtores[i], NULL);

    for (int i = 0; i < NUM_CONSUMIDORES; i++)
        pthread_join(consumidores[i], NULL);

    return 0;
}
