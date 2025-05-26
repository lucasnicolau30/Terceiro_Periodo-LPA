Mini-Laboratório 0 — Exercício 02: Busca Binaria

Arquivo fonte: mlab0-ex02-busca_binaria.c

Compilação:
  	make

Execução:
	Após a compilação, você pode executar o programa passando o valor a ser buscado como argumento:
  	./mlab0-ex02-busca_binaria <valor>

Descrição:
	Neste exercício, o programa realiza uma busca binária paralela em um vetor ordenado utilizando duas threads. A busca binária é um algoritmo eficiente para encontrar um 	valor em um vetor ordenado, dividindo repetidamente o vetor ao meio e comparando o valor alvo com o valor do meio.

Exemplo de Execução:
	Comando:
		./mlab0-ex02-busca_binaria 25

	Saída esperada:
		Valor 25 estava na posição 6 e foi a thread 2 que encontrou.
