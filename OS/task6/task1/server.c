#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>

int fd, sum;
char* fifo = "fifo";


void handler(int i)
{
	close(fd);
	unlink(fifo);

	printf("\nTotal sum = %d\n", sum);

	exit(0);
}


int main(int argc, char** argv)
{
    signal(SIGINT, handler);

	int  n, number;

    sum = 0;

	mkfifo(fifo, 0666);

	fd = open(fifo, O_RDONLY | O_NONBLOCK);

    while ((n = read(fd, &number, sizeof(int))) != -1)
	{
		if (n > 0)
		{
			printf("Server got %d (read return %d)\n", number, n);
            sum += number;
		}
		else
		{
			fprintf(stderr, "Server: read return %d\n", n);
		}

		sleep(1);
	}
    
    return 0;
}
