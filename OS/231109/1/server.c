#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char** argv)
{
	int fd, pid;

	mkfifo("fifo", 0666);

	fd = open("fifo", O_RDONLY | O_NONBLOCK);

	int n;

	// for (int i = 0; i < 3; ++i)
	// {
	// 	p = read(fd, &pid, sizeof(int));
	// 	printf("Server %d got message from %d! return %d\n", getpid(), pid, p);
	// }

	while ((n = read(fd, &pid, sizeof(int))) != -1)
	{
		if (n > 0)
		{
			printf("Server %d got message from %d! return %d\n", getpid(), pid, n);
		}
		else
		{
			fprintf(stderr, "got %d\n", n);
		}

		sleep(1);
	}

	close(fd);
	unlink("fifo");

	fprintf(stderr, "finish %d\n", n);

	return 0;
}