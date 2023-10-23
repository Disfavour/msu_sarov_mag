#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


int main(int argc, char** argv)
{
	printf("father PID=%d, PPID=%d\n", getpid(), getppid());

	for (int i = 0; i < 5; ++i)
	{
		if (fork() == 0)
		{
			printf("son %d PID=%d, PPID=%d\n", i, getpid(), getppid());
			return 0;
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		wait(NULL);
	}

	printf("father PID=%d, PPID=%d\n", getpid(), getppid());

	return 0;
}
