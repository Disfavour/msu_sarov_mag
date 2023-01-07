run: a.out
	./a.out

a.out: main.f08 module.o tb.o
	gfortran main.f08 module.o tb.o

module.o: module.c
	gcc -c module.c

tb.o: tb.f08
	gfortran -c tb.f08
