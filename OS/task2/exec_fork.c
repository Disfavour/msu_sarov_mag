#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


int main()
{
	if (!fork())
	{
		execlp("gcc", "gcc", "1.c", "-o", "1.exe", "-lm", (char*)0);
		exit(1);
	}
	
	wait(NULL);

	return 0;
}
