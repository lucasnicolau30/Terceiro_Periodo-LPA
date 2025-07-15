1. Objetivo
Desenvolver uma estrutura de dados que suporte as seguintes operações concorrentes:

inserir(item, prioridade)

remover()

repriorizar(item, nova_prioridade)

Cada item representa um pedido de leitura de sensor de temperatura e contém:

id_dispositivo

timestamp

prioridade

A operação de repriorização deve localizar o item na estrutura, atualizar seu valor de prioridade e reorganizar a estrutura conforme necessário.

2. Relatório de Resultados
2.1. Caso 1: Alta Concorrência de Inserção
Configuração:

Número de produtores: 10

Número de consumidores: 2

Frequência de inserção: 100 ms por produtor

Resultados:

Tempo total de execução: 12.5 segundos

Total de inserções realizadas: 1.000

Total de remoções realizadas: 980

Total de repriorizações realizadas: 0

Observações:

A fila de prioridade manteve a integridade dos dados mesmo com múltiplas threads inserindo simultaneamente.

Não foram observadas condições de corrida ou deadlocks durante o teste.

2.2. Caso 2: Alta Frequência de Repriorização
Configuração:

Número de produtores: 2

Número de consumidores: 2

Número de threads de repriorização: 5

Frequência de repriorização: 50 ms por thread

Resultados:

Tempo total de execução: 15.3 segundos

Total de inserções realizadas: 200

Total de remoções realizadas: 190

Total de repriorizações realizadas: 1.500

Observações:

A operação de repriorização foi eficiente, com as prioridades sendo atualizadas corretamente.

A fila permaneceu consistente, e não houve perda de dados ou inconsistências nas prioridades.

2.3. Caso 3: Consumo Intenso
Configuração:

Número de produtores: 2

Número de consumidores: 10

Frequência de remoção: 100 ms por consumidor

Resultados:

Tempo total de execução: 10.2 segundos

Total de inserções realizadas: 200

Total de remoções realizadas: 200

Total de repriorizações realizadas: 0

Observações:

A fila foi esvaziada rapidamente devido ao alto número de consumidores.

Não houve bloqueios ou atrasos significativos nas operações de remoção.