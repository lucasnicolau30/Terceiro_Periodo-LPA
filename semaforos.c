#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct Node {
    int valor;
    struct Node* esquerda;
    struct Node* direita;
} Node;

Node* raiz = NULL;
sem_t mutex_leitor;   // protege contador de leitores
sem_t sem_escritor;   // bloqueia escritores
int contador_leitores = 0;

// Funções da árvore
Node* novo_no(int valor) {
    Node* n = malloc(sizeof(Node));
    n->valor = valor;
    n->esquerda = n->direita = NULL;
    return n;
}

void inserir(Node** raiz, int valor) {
    if (*raiz == NULL) {
        *raiz = novo_no(valor);
    } else if (valor < (*raiz)->valor) {
        inserir(&((*raiz)->esquerda), valor);
    } else {
        inserir(&((*raiz)->direita), valor);
    }
}

void em_ordem(Node* n) {
    if (n != NULL) {
        em_ordem(n->esquerda);
        printf("%d ", n->valor);
        em_ordem(n->direita);
    }
}

void* thread_ler(void* arg) {
    // entrada de leitor
    sem_wait(&mutex_leitor);
    contador_leitores++;
    if (contador_leitores == 1) {
        sem_wait(&sem_escritor); // primeiro leitor bloqueia escritores
    }
    sem_post(&mutex_leitor);

    // leitura
    printf("[Leitor] Percorrendo árvore: ");
    em_ordem(raiz);
    printf("\n");
    sleep(1);

    // saída de leitor
    sem_wait(&mutex_leitor);
    contador_leitores--;
    if (contador_leitores == 0) {
        sem_post(&sem_escritor); // último leitor libera escritores
    }
    sem_post(&mutex_leitor);

    return NULL;
}

void* thread_inserir(void* arg) {
    int valor;
    sem_wait(&sem_escritor);
    srand(time(NULL));
    for(int i = 0; i < 10; i ++){
        valor = rand() % 50;
        printf("[Escritor] Inserindo %d\n", valor);
        inserir(&raiz, valor);
        sleep(1); // simula trabalho
    }
    sem_post(&sem_escritor); // libera acesso
    return NULL;
}

int main() {
    pthread_t escritores[10], leitores[10];
    int val1 = 10, val2 = 20;

    sem_init(&mutex_leitor, 0, 1);
    sem_init(&sem_escritor, 0, 1);

    // Cria escritores
    for (int i = 0; i < 10; i++) {
        pthread_create(&escritores[i], NULL, thread_inserir, NULL);
    }
    sleep(2); // deixa tempo para inserção antes da leitura

    // Cria leitores
    for (int i = 0; i < 10; i++) {
        pthread_create(&leitores[i], NULL, thread_ler, NULL);
    }

    for (int i = 0; i < 10; i++) pthread_join(escritores[i], NULL);
    for (int i = 0; i < 10; i++) pthread_join(leitores[i], NULL);

    sem_destroy(&mutex_leitor);
    sem_destroy(&sem_escritor);
    return 0;
}
