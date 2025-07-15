#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>
#include "../common/protocol.h"

#define PORTA 8080

char meu_tabuleiro[BOARD_SIZE][BOARD_SIZE];
char tabuleiro_inimigo[BOARD_SIZE][BOARD_SIZE];
char nome_jogador[50];
bool meu_turno = false;

void limpar_tela() {
    system("clear || cls"); 
}

void mostrar_tabuleiro(char tabuleiro[BOARD_SIZE][BOARD_SIZE], bool mostrar_navios) {
    printf("\n   ");
    for (int i = 0; i < BOARD_SIZE; i++) printf("%d ", i);
    printf("\n");
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d |", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (!mostrar_navios && tabuleiro[i][j] == 'N') {
                printf("~ ");
            } else {
                printf("%c ", tabuleiro[i][j]);
            }
        }
        printf("\n");
    }
}

void mostrar_legenda() {
    printf("\nLegenda:\n");
    printf("~ - Água\n");
    printf("N - Navio (apenas no seu tabuleiro)\n");
    printf("X - Acerto\n");
    printf("O - Tiro na água\n");
}

void inicializar_tabuleiro_cliente(char tabuleiro[BOARD_SIZE][BOARD_SIZE]) {
    memset(tabuleiro, '~', BOARD_SIZE * BOARD_SIZE);
}

void posicionar_navios(int socket) {
    int navios_posicionados[MAX_SHIPS] = {0};
    char comando[MAX_MSG];
    char resposta[MAX_MSG];
    
    inicializar_tabuleiro_cliente(meu_tabuleiro);
    inicializar_tabuleiro_cliente(tabuleiro_inimigo);

    memset(resposta, 0, MAX_MSG);
    read(socket, resposta, MAX_MSG); 
    if (strstr(resposta, "INICIAR_POSICIONAMENTO") == NULL) {
        printf("Erro: Não recebeu o comando INICIAR_POSICIONAMENTO do servidor.\n");
        exit(1);
    }

    while (1) {
        limpar_tela();
        printf("=== FASE DE POSICIONAMENTO ===\n");
        printf("Jogador: %s\n", nome_jogador);
        
        printf("\nNavios a posicionar:\n");
        printf("- Submarino (1x1): %d/%d\n", navios_posicionados[0], QTD_SUBMARINO);
        printf("- Fragata (2x1): %d/%d\n", navios_posicionados[1], QTD_FRAGATA);
        printf("- Destroyer (3x1): %d/%d\n", navios_posicionados[2], QTD_DESTROYER);
        
        printf("\nSeu tabuleiro:\n");
        mostrar_tabuleiro(meu_tabuleiro, true);
        mostrar_legenda();
        
        printf("\nComandos disponíveis:\n");
        printf("1. POSICIONAR <tipo> <linha> <coluna> <H/V>\n");
        printf("   Tipos: 0-Submarino (1x1), 1-Fragata (2x1), 2-Destroyer (3x1)\n");
        printf("   Exemplo: POSICIONAR 1 3 4 H (Fragata na linha 3, coluna 4, horizontal)\n");
        printf("2. PRONTO - Finalizar posicionamento (quando todos os navios estiverem posicionados)\n");
        printf("\n> ");
        
        if (fgets(comando, MAX_MSG, stdin) == NULL) {
            printf("Erro ao ler comando.\n");
            break;
        }
        comando[strcspn(comando, "\n")] = 0;

        if (strncmp(comando, CMD_POSICIONAR, strlen(CMD_POSICIONAR)) == 0) {
            int tipo, x, y;
            char orient_char;
            if (sscanf(comando, "%*s %d %d %d %c", &tipo, &x, &y, &orient_char) != 4) {
                printf("Entrada inválida. Use o formato: POSICIONAR <tipo> <linha> <coluna> <H/V>\n");
                sleep(2);
                continue;
            }

            char orient = (char)toupper(orient_char);

            if (tipo < 0 || tipo >= MAX_SHIPS) {
                printf("Tipo de navio inválido. Use 0, 1 ou 2.\n");
                sleep(2);
                continue;
            }
            if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
                printf("Coordenadas fora do tabuleiro (0-7).\n");
                sleep(2);
                continue;
            }
            if (orient != 'H' && orient != 'V') {
                printf("Orientação inválida. Use H (Horizontal) ou V (Vertical).\n");
                sleep(2);
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
                printf("Você já posicionou o número máximo para este tipo de navio.\n");
                sleep(2);
                continue;
            }
            
            bool client_pos_valid = true;
            if (orient == 'H') {
                if (y + tamanho > BOARD_SIZE) client_pos_valid = false;
                for (int i = 0; i < tamanho && client_pos_valid; i++) {
                    if (meu_tabuleiro[x][y+i] != '~') client_pos_valid = false;
                }
            } else {
                if (x + tamanho > BOARD_SIZE) client_pos_valid = false;
                for (int i = 0; i < tamanho && client_pos_valid; i++) {
                    if (meu_tabuleiro[x+i][y] != '~') client_pos_valid = false;
                }
            }

            if (!client_pos_valid) {
                 printf("Posição inválida ou navio já posicionado localmente.\n");
                 sleep(2);
                 continue;
            }

            send(socket, comando, strlen(comando), 0);
            
            memset(resposta, 0, MAX_MSG);
            if (read(socket, resposta, MAX_MSG) <= 0) {
                printf("Servidor desconectou.\n");
                break;
            }
            
            if (strstr(resposta, "OK")) {
                if (orient == 'H') {
                    for (int i = 0; i < tamanho; i++) meu_tabuleiro[x][y+i] = 'N';
                } else {
                    for (int i = 0; i < tamanho; i++) meu_tabuleiro[x+i][y] = 'N';
                }
                navios_posicionados[tipo]++;
                printf("Navio posicionado com sucesso! %s\n", resposta);
            } else {
                printf("Erro do servidor: %s\n", resposta);
            }
            sleep(1);
        } 
        else if (strcmp(comando, "PRONTO") == 0) {
            if (navios_posicionados[0] >= QTD_SUBMARINO &&
                navios_posicionados[1] >= QTD_FRAGATA &&
                navios_posicionados[2] >= QTD_DESTROYER) {
                
                send(socket, CMD_PRONTO "\n", strlen(CMD_PRONTO "\n"), 0);
                memset(resposta, 0, MAX_MSG);
                if (read(socket, resposta, MAX_MSG) <= 0) {
                    printf("Servidor desconectou.\n");
                    break;
                }
                printf("%s\n", resposta);
                if (strstr(resposta, "PRONTO_OK")) {
                    break;
                } else {
                    printf("Erro ao finalizar posicionamento: %s\n", resposta);
                    sleep(2);
                }
            } else {
                printf("Você ainda não posicionou todos os navios necessários!\n");
                sleep(2);
            }
        } else {
            printf("Comando desconhecido. Por favor, use 'POSICIONAR' ou 'PRONTO'.\n");
            sleep(2);
        }
    }
}

void jogar(int socket) {
    char buffer[MAX_MSG];
    
    while (1) {
        memset(buffer, 0, MAX_MSG);
        int bytes = read(socket, buffer, MAX_MSG);
        if (bytes <= 0) {
            printf("Servidor desconectou. Fim de jogo.\n");
            break;
        }
        
        buffer[bytes] = '\0';

        if (strstr(buffer, CMD_JOGADOR)) {
            meu_turno = true;
            limpar_tela();
            printf("=== SEU TURNO ===\n");
            printf("Jogador: %s\n", nome_jogador);
            
            printf("\nSeu tabuleiro:\n");
            mostrar_tabuleiro(meu_tabuleiro, true);
            
            printf("\nTabuleiro inimigo:\n");
            mostrar_tabuleiro(tabuleiro_inimigo, false);
            mostrar_legenda();
            
            printf("\nDigite as coordenadas para atacar (linha coluna): ");
            int x, y;
            while (scanf("%d %d", &x, &y) != 2 || x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || getchar() != '\n') {
                printf("Coordenadas inválidas ou formato incorreto! Digite novamente (linha coluna): ");
                while (getchar() != '\n');
            }
            
            sprintf(buffer, CMD_ATIRAR " %d %d\n", x, y);
            send(socket, buffer, strlen(buffer), 0);
            
            memset(buffer, 0, MAX_MSG);
            bytes = read(socket, buffer, MAX_MSG);
            if (bytes <= 0) {
                printf("Servidor desconectou após seu ataque. Fim de jogo.\n");
                break;
            }
            buffer[bytes] = '\0';

            if (strstr(buffer, CMD_ACERTOU)) {
                printf("Você acertou um navio inimigo em (%d,%d)!\n", x, y);
                tabuleiro_inimigo[x][y] = 'X';
            } else if (strstr(buffer, CMD_AGUA)) {
                printf("Você atingiu a água em (%d,%d)!\n", x, y);
                tabuleiro_inimigo[x][y] = 'O';
            } else if (strstr(buffer, "ERRO")) {
                printf("Erro ao atacar: %s\n", buffer);
                continue;
            }
            sleep(2);

        } else if (strstr(buffer, CMD_ESPERAR)) {
            meu_turno = false;
            limpar_tela();
            printf("=== AGUARDANDO TURNO DO ADVERSÁRIO ===\n");
            sleep(1);
        } else if (strstr(buffer, CMD_VITORIA)) {
            limpar_tela();
            printf("\nPARABÉNS! VOCÊ VENCEU O JOGO!\n");
            
            printf("\nTabuleiro final do inimigo:\n");
            mostrar_tabuleiro(tabuleiro_inimigo, true);
            
            break;
        } else if (strstr(buffer, CMD_DERROTA)) {
            limpar_tela();
            printf("\nVOCÊ PERDEU! MELHOR SORTE NA PRÓXIMA!\n");
            
            printf("\nSeu tabuleiro final:\n");
            mostrar_tabuleiro(meu_tabuleiro, true);
            
            break;
        } else {
            printf("Mensagem desconhecida do servidor: %s\n", buffer);
            sleep(1);
        }
    }
}

int main(int argc, char *argv[]) {
    char *server_ip = "127.0.0.1";
    if (argc > 1) {
        server_ip = argv[1];
    }

    int socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_cliente < 0) {
        perror("Erro ao criar socket do cliente");
        return 1;
    }
    
    struct sockaddr_in servidor = {
        .sin_family = AF_INET,
        .sin_port = htons(PORTA)
    };
    
    if (inet_pton(AF_INET, server_ip, &servidor.sin_addr) <= 0) {
        fprintf(stderr, "Endereço do servidor inválido ou não suportado: %s\n", server_ip);
        close(socket_cliente);
        return 1;
    }
    
    printf("Conectando ao servidor em %s:%d...\n", server_ip, PORTA);
    if (connect(socket_cliente, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        perror("Erro ao conectar ao servidor");
        close(socket_cliente);
        return 1;
    }
    printf("Conectado ao servidor!\n");
    
    printf("Bem-vindo ao Batalha Naval!\nDigite seu nome: ");
    if (fgets(nome_jogador, sizeof(nome_jogador), stdin) == NULL) {
        printf("Erro ao ler o nome.\n");
        close(socket_cliente);
        return 1;
    }
    nome_jogador[strcspn(nome_jogador, "\n")] = 0;
    
    char buffer[MAX_MSG];
    sprintf(buffer, CMD_ENTRAR " %s\n", nome_jogador);
    send(socket_cliente, buffer, strlen(buffer), 0);
    
    posicionar_navios(socket_cliente);
    
    jogar(socket_cliente);
    
    close(socket_cliente);
    printf("\nJogo encerrado. Pressione Enter para sair...");
    getchar();
    return 0;
}