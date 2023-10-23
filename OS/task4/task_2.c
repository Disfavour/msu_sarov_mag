#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


int main(int argc, char** argv)
{
	if (argc != 4)
	{
		// pr1 | pr2 > f.res 
		printf("Use: %s pr1 pr2 f.res\n", argv[0]);
		return -1;
	}

	int fd[2];

	pipe(fd);

	if (!fork())
	{
		close(fd[0]);

		dup2(fd[1], 1);
		close(fd[1]);
		
		execlp(argv[1], argv[1], NULL);
	}

	close(fd[1]);

	int f = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if (!fork())
	{
		dup2(fd[0], 0);
		close(fd[0]);

		dup2(f, 1);
		close(f);

		execlp(argv[2], argv[2], NULL);
	}

	close(fd[0]);
	close(f);

	for (int i = 0; i < 2; ++i)
	{
		wait(NULL);
	}
	
	return 0;
}
