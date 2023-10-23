#include <unistd.h>
#include <signal.h>
#include <stdio.h>


void alrm (int s)
{
	printf("\nfaster\n");
}


int main()
{
	char s[80];
	int pid;

	signal(SIGALRM, alrm);

	if (!(pid = fork()))
	{
		printf("Enter name \n");

		for (;;)
		{
			printf("name: ");

			if (gets(s) != NULL)
				break;
		}

		printf("OK! \n");

		kill(getppid(), SIGKILL);
	}
	else
	{
		for (;;)
		{
			sleep(5);
			kill(pid, SIGALRM);
		}
	}

	return 0;
}
