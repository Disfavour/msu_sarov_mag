#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		// pr1 && pr2
		printf("Use: %s pr1 pr2\n", argv[0]);
		return 0;
	}

	if (!fork())
	{
		execlp(argv[1], argv[1], NULL);
	}

	int status;

	wait(&status);

	if (WIFEXITED(status) && !WEXITSTATUS(status))
	{
		if (!fork())
		{
			execlp(argv[2], argv[2], NULL);
		}

		wait(NULL);
	}
	
	return 0;
}
