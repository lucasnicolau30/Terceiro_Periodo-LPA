Mini-Laboratório 0 — Exercício 03: Soma Vetor

Arquivo fonte: mlab0-ex03-soma_vetor.c

Compilação:
  	make

Execução:
	Após a compilação, você pode executar o programa passando o tamanho do vetor e o número de threads a serem usadas como argumentos:
  	./mlab0-ex03-soma_vetor <tamanho> <num_threads>

Descrição:
	Neste exercício, o programa realiza a soma dos elementos de um vetor utilizando múltiplas threads. O vetor é dividido entre as threads e cada thread calcula a soma de 	uma parte do vetor. Ao final, a soma total é obtida pela soma das somas parciais de cada thread.

Exemplo de Execução:
	Comando:
		./mlab0-ex03-soma_vetor 100 4
	Saída esperada:
		Vetor gerado:
			34 57 23 65 12 74 28 9 30 45 86 13 90 4 31 59 10 29 72 58 37 14 6 61 88 19 67 36 28 51 98 92 46 68 17 75 85 69 28 7 78 52 76 82 53 37 54 81 3 80 28 60 				25 55 43 57 4 93 41 72 16 85 23 45 10 98 34 99 19 93 44 91 44 56 8 67 80 39 17 99 57 77 34 62 41 72 57 81 42 40 78 92 33 76 11 5 71 39 48 26 11 24 57 61 			65 50 94 21 60 38 93 66 95 58 14 93 68 25
			Soma total: 4850
