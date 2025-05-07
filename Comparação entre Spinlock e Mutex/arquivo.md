Tabela:

## Tabela Comparativa de Desempenho

| Nº de Threads | Tempo Spinlock | Tempo Mutex | Contador Correto  | Observações          |
|---------------|----------------|-------------|-------------------|----------------------|
| 2             | 0.14s          | 0.22s       | Sim               |                      |
| 4             | 0.79s          | 1.09s       | Sim               |                      |
| 8             | 7.17s          | 3.69s       | Sim               | Mutex mais rápido    |
| 16            | 22.51s         | 18.02s      | Sim               | Mutex mais rápido    |

## Respostas às Perguntas

### 1. Quando usar spinlock?

O spinlock é mais adequado quando:
- As regiões críticas são *curtas*, ou seja, a operação dentro do lock leva pouco tempo.
- A contenda entre threads é *baixa*, com poucas threads tentando acessar a região crítica simultaneamente.
- O *tempo de bloqueio do mutex* é mais custoso do que o overhead de manter a thread em "busy-wait" (spinlock).
- O sistema não pode *dormir ou bloquear threads*, o que é comum em sistemas de tempo real ou sistemas embarcados.

*Exemplo de uso*: Estruturas de dados simples, contadores rápidos e flags de sinalização com baixa concorrência.

---

### 2. Quando o mutex é mais adequado?

O mutex é mais adequado quando:
- As regiões críticas são *mais longas*, ou seja, o tempo de execução das operações dentro do lock é significativo.
- Há *alta contenda*, com muitas threads tentando acessar a mesma região crítica simultaneamente.
- Você quer *evitar o uso excessivo de CPU*, pois o mutex coloca a thread em estado de bloqueio (sleep) enquanto espera, ao invés de consumir recursos em "busy-wait".

*Exemplo de uso*: Acesso a arquivos, manipulação de dados grandes, ou operações que envolvem I/O ou cálculos demorados.

---

### 3. Melhor eficiência no experimento?

- *Com 2 e 4 threads*: O mutex teve melhor desempenho, sendo mais rápido.
- *Com 8 threads*: O mutex ainda foi mais rápido, mas a diferença não é tão grande.
- *Com 16 threads*: O mutex foi novamente mais rápido que o spinlock, apesar do aumento da concorrência.

*Conclusão*: O mutex se mostrou mais eficiente no experimento, especialmente quando o número de threads aumentou. Mesmo com 16 threads, o mutex foi mais rápido, o que pode ser um reflexo da melhor gestão de bloqueios que ele oferece, comparado ao custo de "busy-wait" do spinlock.

---

### 4. Qual foi o resultado da execução sem sincronização?

Sem qualquer mecanismo de sincronização (lock/unlock), o programa apresentou *resultados incorretos. O contador final foi **menor que o esperado, pois as múltiplas threads competiram para acessar e modificar a variável contador simultaneamente, resultando em **condições de corrida*.

*Conclusão*: A falta de sincronização resultou em inconsistências nos dados. Isso mostra claramente a necessidade de utilizar mecanismos como spinlock ou mutex para garantir a integridade das operações em programas multithreaded.

---
