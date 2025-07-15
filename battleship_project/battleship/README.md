# Batalha Naval Multicliente em C – Nathãn Barbosa e Lucas Nicolau

Este projeto implementa um jogo de Batalha Naval para dois jogadores, utilizando a linguagem C com sockets TCP e threads. O servidor gerencia as conexões e a lógica do jogo, enquanto cada jogador se conecta como cliente e interage via terminal.

## Estrutura do Projeto

├── Makefile  ## Script para compilar servidor e cliente
├── common/
│ └── protocol.h  ## Definição do protocolo de comunicação
├── server/
│ └── battleserver.c  ## Código fonte do servidor
├── client/
│ └── battleclient.c   ## Código fonte do cliente
└── README.md  ## Arquivo de documentação


## Compilação (Linux)

Para compilar o servidor e o cliente, utilize o comando abaixo no terminal, dentro da pasta do projeto:


Os executáveis serão gerados nos respectivos diretórios:

- `server/battleserver`
- `client/battleclient`

Para limpar os binários gerados:

make clean


## Execução

### Servidor

Inicie o servidor com o comando:

./server/battleserver

O servidor ficará aguardando a conexão de dois jogadores na porta 8080.

### Cliente

Cada jogador deve abrir um terminal separado e executar:

./client/battleclient


O cliente solicitará o nome do jogador, o posicionamento dos navios e iniciará a partida assim que ambos estiverem prontos.

## Regras do Jogo

Cada jogador deve posicionar:

- 1 Submarino (1x1)
- 2 Fragatas (2x1)
- 1 Destroyer (3x1)

Comando para posicionar:

POSICIONAR <tipo> <linha> <coluna> <H/V>

- H para horizontal 
-V para vertical

Exemplo:

POSICIONAR 1 3 4 H

Após posicionar todos os navios, o jogador deve digitar:

PRONTO

## Fluxo do Jogo

- O servidor controla o turno dos jogadores, alternando entre eles.
- O jogador ativo recebe o comando para atacar, inserindo as coordenadas.
- O servidor verifica se o ataque acertou um navio, água, ou se a posição já foi atacada.
- O jogo termina quando todos os navios de um jogador forem afundados.

## Protocolo de Comunicação

Os comandos trocados entre cliente e servidor seguem o protocolo definido em `common/protocol.h`. Exemplos:

- Mensagens específicas indicam acerto, erro, vitória ou derrota.
- `ENTRAR <nome>` – Envia o nome do jogador ao servidor
- `POSICIONAR` – Posiciona um navio
- `PRONTO` – Confirma que todos os navios foram posicionados
- `ATIRAR <x> <y>` – Realiza um ataque
- `ACERTOU`, `AGUA`, `VITORIA`, `DERROTA` – Respostas do servidor
- `JOGADOR` – Informa que é o turno do jogador
- `ESPERAR` – Indica que o jogador deve aguardar

## Observações

- A troca de mensagens ocorre via TCP
- O projeto utiliza `pthread` para tratar múltiplas conexões simultâneas
- O código foi testado e funciona corretamente em sistemas Linux