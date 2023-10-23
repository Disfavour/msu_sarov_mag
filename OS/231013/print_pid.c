#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	printf("PID=%d, PPID=%d\n", getpid(), getppid());

	for (int i = 0; i < 2; ++i)
	{
		sleep(1);
		fork();
		printf("PID=%d, PPID=%d\n", getpid(), getppid());
	}

	//printf("PID=%d, PPID=%d\n", getpid(), getppid());

	return 0;
}