#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


#define N 10


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Use: %s f1 f2", argv[0]);
	}

	int f1 = open(argv[1], O_RDONLY);
	int f2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	int k;

	char buf[N];

	while ((k = read(f1, buf, N)) > 0)
	{
		write(f2, buf, k);
	}

	close(f1);
	close(f2);
}