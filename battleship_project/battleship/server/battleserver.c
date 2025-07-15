#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include "../common/protocol.h"

#define PORTA 8080
#define MAX_JOGADORES 2

typedef struct {
    int socket;
    char nome[50];
    char tabuleiro[BOARD_SIZE][BOARD_SIZE];
    int navios_restantes;
} Jogador;

Jogador jogadores[MAX_JOGADORES];
int total_jogadores = 0;
bool jogo_iniciado = false;
pthread_mutex_t mutex;

void enviar_mensagem(int socket, const char *mensagem);
void inicializar_tabuleiro(char tabuleiro[BOARD_SIZE][BOARD_SIZE]);
bool posicao_valida(char tabuleiro[BOARD_SIZE][BOARD_SIZE], int x, int y, int tamanho, char orientacao);
void posicionar_navio(char tabuleiro[BOARD_SIZE][BOARD_SIZE], int x, int y, int tamanho, char orientacao);

void inicializar_tabuleiro(char tabuleiro[BOARD_SIZE][BOARD_SIZE]) {
    memset(tabuleiro, '~', BOARD_SIZE * BOARD_SIZE);
}

bool posicao_valida(char tabuleiro[BOARD_SIZE][BOARD_SIZE], int x, int y, int tamanho, char orientacao) {
    if (orientacao == 'H' || orientacao == 'h') {
        if (y + tamanho > BOARD_SIZE) return false;
        for (int i = 0; i < tamanho; i++) {
            if (tabuleiro[x][y+i] != '~') return false;
        }
    } else if (orientacao == 'V' || orientacao == 'v') {
        if (x + tamanho > BOARD_SIZE) return false;
        for (int i = 0; i < tamanho; i++) {
            if (tabuleiro[x+i][y] != '~') return false;
        }
    } else {
        return false;
    }
    return true;
}

void posicionar_navio(char tabuleiro[BOARD_SIZE][BOARD_SIZE], int x, int y, int tamanho, char orientacao) {
    if (orientacao == 'H' || orientacao == 'h') {
        for (int i = 0; i < tamanho; i++) tabuleiro[x][y+i] = 'N';
    } else if (orientacao == 'V' || orientacao == 'v') {
        for (int i = 0; i < tamanho; i++) tabuleiro[x+i][y] = 'N';
    }
}

void enviar_mensagem(int socket, const char *mensagem) {
    send(socket, mensagem, strlen(mensagem), 0);
}

void *gerenciar_jogador(void *arg) {
    int id = *((int *)arg);
    int socket = jogadores[id].socket;
    char buffer[MAX_MSG];

    memset(buffer, 0, MAX_MSG);
    read(socket, buffer, MAX_MSG);
    sscanf(buffer, "%*s %s", jogadores[id].nome);
    printf("[SERVIDOR] Jogador %d conectado: %s\n", id+1, jogadores[id].nome);

    inicializar_tabuleiro(jogadores[id].tabuleiro);
    enviar_mensagem(socket, "INICIAR_POSICIONAMENTO\n");

    int navios_posicionados[MAX_SHIPS] = {0};

    while (1) {
        memset(buffer, 0, MAX_MSG);
        int bytes_read = read(socket, buffer, MAX_MSG);
        if (bytes_read <= 0) {
            printf("[SERVIDOR] Jogador %d desconectou durante posicionamento.\n", id+1);
            break;
        }

        if (strncmp(buffer, CMD_POSICIONAR, strlen(CMD_POSICIONAR)) == 0) {
            int tipo, x, y;
            char orient;
            if (sscanf(buffer, "%*s %d %d %d %c", &tipo, &x, &y, &orient) != 4) {
                enviar_mensagem(socket, "ERRO Formato de comando POSICIONAR inválido.\n");
                continue;
            }

            if (tipo < 0 || tipo >= MAX_SHIPS) {
                enviar_mensagem(socket, "ERRO Tipo de navio inválido (0-Submarino, 1-Fragata, 2-Destroyer).\n");
                continue;
            }

            int tamanho = 0;
            int max_count = 0;
            switch (tipo) {
                case 0: tamanho = 1; max_count = QTD_SUBMARINO; break;
                case 1: tamanho = 2; max_count = QTD_FRAGATA; break;
                case 2: tamanho = 3; max_count = QTD_DESTROYER; break;
            }

            if (navios_posicionados[tipo] >= max_count) {
                enviar_mensagem(socket, "ERRO Você já posicionou o número máximo para este tipo de navio.\n");
                continue;
            }
            
            if (posicao_valida(jogadores[id].tabuleiro, x, y, tamanho, orient)) {
                posicionar_navio(jogadores[id].tabuleiro, x, y, tamanho, orient);
                navios_posicionados[tipo]++;

                char resposta[150];
                sprintf(resposta, "OK Navio posicionado. Faltam: %d Submarinos, %d Fragatas, %d Destroyers\n",
                               QTD_SUBMARINO - navios_posicionados[0],
                               QTD_FRAGATA - navios_posicionados[1],
                               QTD_DESTROYER - navios_posicionados[2]);
                enviar_mensagem(socket, resposta);
            } else {
                enviar_mensagem(socket, "ERRO Posição inválida ou sobreposição com outro navio/fora do tabuleiro.\n");
            }
        }
        else if (strncmp(buffer, CMD_PRONTO, strlen(CMD_PRONTO)) == 0) {
            if (navios_posicionados[0] >= QTD_SUBMARINO &&
                navios_posicionados[1] >= QTD_FRAGATA &&
                navios_posicionados[2] >= QTD_DESTROYER) {

                jogadores[id].navios_restantes = (QTD_SUBMARINO * 1) + (QTD_FRAGATA * 2) + (QTD_DESTROYER * 3);
                enviar_mensagem(socket, "PRONTO_OK Todos os navios posicionados! Aguarde...\n");
                break;
            } else {
                enviar_mensagem(socket, "ERRO Você ainda não posicionou todos os navios necessários!\n");
            }
        } else {
            enviar_mensagem(socket, "ERRO Comando desconhecido durante o posicionamento.\n");
        }
    }

    pthread_mutex_lock(&mutex);
    total_jogadores++;

    if (total_jogadores == MAX_JOGADORES) {
        printf("[SERVIDOR] Ambos jogadores prontos. Iniciando jogo!\n");
        jogo_iniciado = true;

        enviar_mensagem(jogadores[0].socket, CMD_JOGADOR "\n");
        enviar_mensagem(jogadores[1].socket, CMD_ESPERAR "\n");
    }
    pthread_mutex_unlock(&mutex);

    while (!jogo_iniciado) {
        usleep(100000);
    }

    while (jogo_iniciado) {
        memset(buffer, 0, MAX_MSG);
        int bytes = read(socket, buffer, MAX_MSG);
        if (bytes <= 0) {
            printf("[SERVIDOR] Jogador %d desconectou durante o jogo.\n", id+1);
            if (jogo_iniciado) {
                int oponente = 1 - id;
                enviar_mensagem(jogadores[oponente].socket, CMD_VITORIA "\n");
                printf("[SERVIDOR] Jogador %d desconectou. Jogador %d venceu por desistência.\n", id+1, oponente+1);
                jogo_iniciado = false;
            }
            break;
        }

        if (strncmp(buffer, CMD_ATIRAR, strlen(CMD_ATIRAR)) == 0) {
            int x, y;
            if (sscanf(buffer, "%*s %d %d", &x, &y) != 2) {
                enviar_mensagem(socket, "ERRO Formato de comando ATIRAR inválido.\n");
                enviar_mensagem(socket, CMD_JOGADOR "\n");
                continue;
            }

            if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                enviar_mensagem(socket, "ERRO Coordenadas de ataque fora do tabuleiro.\n");
                enviar_mensagem(socket, CMD_JOGADOR "\n");
                continue;
            }
            
            int oponente = 1 - id;
            
            pthread_mutex_lock(&mutex); 
            printf("[SERVIDOR] Jogador %d atacou (%d,%d) no tabuleiro do Jogador %d.\n", id+1, x, y, oponente+1);
            
            if (jogadores[oponente].tabuleiro[x][y] == 'X' || jogadores[oponente].tabuleiro[x][y] == 'O') {
                enviar_mensagem(socket, "ERRO Você já atirou nesta posição. Escolha outra.\n");
                pthread_mutex_unlock(&mutex);
                enviar_mensagem(socket, CMD_JOGADOR "\n");
                continue;
            }

            if (jogadores[oponente].tabuleiro[x][y] == 'N') {
                jogadores[oponente].tabuleiro[x][y] = 'X';
                jogadores[oponente].navios_restantes--;
                
                enviar_mensagem(socket, CMD_ACERTOU "\n");
                
                if (jogadores[oponente].navios_restantes == 0) {
                    enviar_mensagem(socket, CMD_VITORIA "\n");
                    enviar_mensagem(jogadores[oponente].socket, CMD_DERROTA "\n");
                    jogo_iniciado = false;
                    printf("[SERVIDOR] Jogador %d venceu!\n", id+1);
                    pthread_mutex_unlock(&mutex);
                    break;
                }
            } else {
                jogadores[oponente].tabuleiro[x][y] = 'O';
                enviar_mensagem(socket, CMD_AGUA "\n");
            }
            
            if (jogo_iniciado) {
                enviar_mensagem(socket, CMD_ESPERAR "\n");
                enviar_mensagem(jogadores[oponente].socket, CMD_JOGADOR "\n");
            }
            pthread_mutex_unlock(&mutex);
        } else {
            enviar_mensagem(socket, "ERRO Comando desconhecido ou não é seu turno.\n");
            if (jogo_iniciado) {
                enviar_mensagem(socket, CMD_ESPERAR "\n");
            }
        }
    }
    
    if (socket > 0) {
        close(socket);
    }
    printf("[SERVIDOR] Conexão com jogador %d encerrada\n", id+1);
    return NULL;
}

int main() {
    int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor < 0) {
        perror("Erro ao criar socket do servidor");
        exit(1);
    }
    
    int optval = 1;
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Erro ao configurar SO_REUSEADDR");
        close(socket_servidor);
        exit(1);
    }

    struct sockaddr_in endereco = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORTA)
    };

    if (bind(socket_servidor, (struct sockaddr*)&endereco, sizeof(endereco)) < 0) {
        perror("Erro ao fazer bind");
        close(socket_servidor);
        exit(1);
    }

    if (listen(socket_servidor, MAX_JOGADORES) < 0) {
        perror("Erro ao escutar no socket");
        close(socket_servidor);
        exit(1);
    }

    printf("[SERVIDOR] Servidor iniciado na porta %d. Aguardando jogadores...\n", PORTA);

    pthread_mutex_init(&mutex, NULL);

    int ids_jogadores[MAX_JOGADORES];
    pthread_t threads_jogadores[MAX_JOGADORES];
    
    for (int i = 0; i < MAX_JOGADORES; i++) {
        jogadores[i].socket = accept(socket_servidor, NULL, NULL);
        if (jogadores[i].socket < 0) {
            perror("Erro ao aceitar conexão");
            i--;
            continue;
        }
        
        ids_jogadores[i] = i;
        
        printf("[SERVIDOR] Aceitando conexão do jogador %d\n", i+1);

        if (pthread_create(&threads_jogadores[i], NULL, gerenciar_jogador, &ids_jogadores[i]) != 0) {
            perror("Erro ao criar thread do jogador");
            close(jogadores[i].socket);
            i--;
            continue;
        }

        printf("[SERVIDOR] Jogador %d conectado. Aguardando %d jogador(es)...\n",
                       i+1, MAX_JOGADORES - (i+1));
    }

    for (int i = 0; i < MAX_JOGADORES; i++) {
        pthread_join(threads_jogadores[i], NULL);
    }

    close(socket_servidor);
    pthread_mutex_destroy(&mutex);
    printf("[SERVIDOR] Servidor encerrado\n");
    return 0;
}