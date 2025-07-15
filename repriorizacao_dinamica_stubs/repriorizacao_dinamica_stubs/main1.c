#include <stdio.h>
#include <string.h>

#define MAX 10

typedef struct {
    char paginas[MAX][100];
    int topo;
} Pilha;

void inicializa(Pilha *p) {
    p->topo = -1;
}

int push(Pilha *p, const char *url) {
    if (p->topo == MAX - 1) return 0;
    p->topo++;
    strcpy(p->paginas[p->topo], url);
    return 1;
}

int pop(Pilha *p, char *url) {
    if (p->topo == -1) return 0;
    strcpy(url, p->paginas[p->topo]);
    p->topo--;
    return 1;
}

int main() {
    Pilha historico;
    inicializa(&historico);
    char url[100];

    // Simulando navegação
    push(&historico, "www.google.com");
    push(&historico, "www.youtube.com");
    push(&historico, "www.github.com");

    printf("Usuário aperta VOLTAR\n");

    if (pop(&historico, url)) {
        printf("Voltando para: %s\n", historico.paginas[historico.topo]);
    }

    return 0;
}
