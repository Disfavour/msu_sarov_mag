#include <stdio.h>


void print_all(char** arr)
{
	for (int i = 0; ; i++)
	{
		if (arr[i] == NULL)
			break;

		printf("%s\n", arr[i]);
	}
}


int main(int argc, char** argv, char** env)
{
	printf("argc\n%d\n", argc);

	printf("\nargv\n");
	print_all(argv);

	printf("\nenv\n");
	print_all(env);

	return 0;
}
