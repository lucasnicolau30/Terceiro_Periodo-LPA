Mini-Laboratório 0 — Exercício 04: Preenchimento Matriz

Arquivo fonte: mlab0-ex04-preenchimento_matriz.c

Compilação:
  	make

Execução:
	Após a compilação, você pode executar o programa passando o número de linhas, colunas e o número de threads a serem usadas como argumentos:
  	./mlab0-ex04-preenchimento_matriz <linhas> <colunas> <num_threads>

Descrição:
	Neste exercício, o programa aloca uma matriz de dimensões N x M dinamicamente e preenche as linhas da matriz usando múltiplas threads. Cada thread é responsável por 	preencher um conjunto de linhas da matriz com números aleatórios. A ideia é paralelizar a tarefa de preenchimento para aproveitar melhor os recursos do sistema e 	reduzir o tempo de execução.

Exemplo de Execução:
	Comando:
		./mlab0-ex04-preenchimento_matriz 5 4 2
	Saída esperada:
	Matriz preenchida:
		48 15 72 23 
		58 44 32 5 
		17 82 25 61 
		35 94 9 13 
		74 24 81 77 
