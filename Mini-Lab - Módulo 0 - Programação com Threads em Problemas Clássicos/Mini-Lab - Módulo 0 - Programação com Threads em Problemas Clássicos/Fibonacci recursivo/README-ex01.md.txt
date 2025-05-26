Mini-Laboratório 0 — Exercício 01: Fibonacci

Arquivo fonte: mlab0-ex01-fibonacci.c

Compilação:
  	make

Execução:
	Após compilar, você pode executar o programa passando um número n como argumento para calcular o n-ésimo número de Fibonacci:
	./mlab0-ex01-fibonacci <n>

Descrição:
	Este programa tem como objetivo calcular o n-ésimo número da sequência de Fibonacci utilizando threads em C. A abordagem adotada neste exercício é recursiva, onde a 	função fibonacci cria novas threads para calcular os dois termos anteriores da sequência (fib(n-1) e fib(n-2)) até atingir os casos base.

Exemplo de Execução:
	Comando:
		./mlab0-ex01-fibonacci 8

	Saída esperada:
		Fibonacci de 8 = 21