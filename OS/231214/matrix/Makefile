CC = gcc
CFLAGS = -c -Wall -pedantic -Ofast

SRCMODULES = matrix.c
OBJMODULES = $(SRCMODULES:.c=.o)

run: app
	./app

app: main.o $(OBJMODULES)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<
matrix.o: matrix.c matrix.h main.c

# %.o: %.c %.h
# 	$(CC) $(CFLAGS) $<

main.o: main.c matrix.h
	$(CC) $(CFLAGS) $<

deps.mk: $(SRCMODULES)
	$(CC) -MM $^ > $@

# ifneq(clean, $(MAKECMDGOALS))
# 	-include deps.mk
# endif

# matrix.o: matrix.c matrix.h
# 	$(CC) $(CFLAGS) $<

clean:
	rm *.o app

# gcc -MM matrix.c
# matrix.o: matrix.c matrix.h

