run: matrix.out
	./matrix.out

matrix.o: matrix.c matrix.h
	gcc -c matrix.c -o matrix.o

matrix.out: main.c matrix.o
	gcc main.c matrix.o -o matrix.out
