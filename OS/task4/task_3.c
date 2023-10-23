#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		// pr1; pr2; ... ; prn 
		printf("Use: %s pr1 pr2 ...  prn\n", argv[0]);
		return 0;
	}

	for (int i = 1; i < argc; ++i)
	{
		if (!fork())
		{
			execlp(argv[i], argv[i], NULL);
		}

		wait(NULL);
	}
	
	return 0;
}
