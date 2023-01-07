run: a.out
	./a.out

a.out: func.o  main.f08
	gfortran func.o  main.f08

func.o: func.c
	gcc -c func.c
