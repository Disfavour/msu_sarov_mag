#include <unistd.h>
#include <signal.h>
#include <stdio.h>


void alrm (int s)
{
	printf("\nweighting for name\n");
	alarm(5);
}


int main()
{
	char s[80];

	signal(SIGALRM, alrm);
	alarm(5);
	printf("Enter name \n");

	for (;;)
	{
		printf("name: ");

		if (gets(s) != NULL)
			break;
	}

	printf("OK! \n");

	return 0;
}