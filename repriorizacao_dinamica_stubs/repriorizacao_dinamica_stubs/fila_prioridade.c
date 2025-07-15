#include "fila_prioridade.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_MAX 100

static Pedido fila[TAM_MAX];
static int tamanho = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void inicializar_fila() {
    pthread_mutex_lock(&mutex);
    tamanho = 0;
    pthread_mutex_unlock(&mutex);
}

void inserir(Pedido p) {
    pthread_mutex_lock(&mutex);
    if (tamanho >= TAM_MAX) {
        printf("Fila cheia. Não é possível inserir.\n");
        pthread_mutex_unlock(&mutex);
        return;
    }

    int i = tamanho - 1;
    while (i >= 0 && fila[i].prioridade < p.prioridade) {
        fila[i + 1] = fila[i];
        i--;
    }
    fila[i + 1] = p;
    tamanho++;
    pthread_mutex_unlock(&mutex);
}

Pedido remover() {
    pthread_mutex_lock(&mutex);
    Pedido resultado;
    if (tamanho == 0) {
        resultado.id_dispositivo = -1;
        resultado.timestamp = 0;
        resultado.prioridade = -1;
    } else {
        resultado = fila[0];
        for (int i = 1; i < tamanho; i++) {
            fila[i - 1] = fila[i];
        }
        tamanho--;
    }
    pthread_mutex_unlock(&mutex);
    return resultado;
}

int repriorizar(int id_dispositivo, int nova_prioridade) {
    pthread_mutex_lock(&mutex);
    int encontrado = 0;
    for (int i = 0; i < tamanho; i++) {
        if (fila[i].id_dispositivo == id_dispositivo) {
            Pedido p = fila[i];
            for (int j = i; j < tamanho - 1; j++) {
                fila[j] = fila[j + 1];
            }
            tamanho--;
            p.prioridade = nova_prioridade;
            inserir(p);
            encontrado = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    return encontrado;
}
