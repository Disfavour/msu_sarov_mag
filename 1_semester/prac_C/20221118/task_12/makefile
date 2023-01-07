run: task.out
	./task.out

matrix.o: matrix.c matrix.h
	gcc -c matrix.c -o matrix.o

task.out: task_1.c matrix.o
	gcc task_1.c matrix.o -o task.out
