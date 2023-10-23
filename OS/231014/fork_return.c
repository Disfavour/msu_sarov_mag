#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main()
{
	for (int i = 0; i < 5; ++i)
	{
		if (!fork())
		{
			//printf("son %d PID=%d, PPID=%d\n", i, getpid(), getppid());
			sleep(5-i);
			return i;
		}
	}

	int status, k;

	for (int i = 0; i < 5; ++i)
	{
		k = wait(&status);

		if (WIFEXITED(status))
		{
			printf("pid = %d, exit code = %d\n", k, WEXITSTATUS(status));
		}
		else
		{
			printf("pid = %d terminated\n", k);
		}
	}

	return 0;
}