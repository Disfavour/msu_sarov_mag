#include <signal.h>
#include <stdio.h>


int count = 1;


void sigHndlr(int s)
{
	printf("\n I got SIGINT %d time(s) \n", count++);

	if (count == 5)
	{
		signal(SIGINT, SIG_DFL);
	}
}


int main()
{
	signal(SIGINT, sigHndlr);

	while (1);

	return 0;
}