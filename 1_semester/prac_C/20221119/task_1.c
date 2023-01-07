#include <stdio.h>


// Количество  'A' в файле
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("No file in argv\n");
		return -1;
	}

	FILE* f = fopen(argv[1], "r");
	
	if (!f)
	{
		printf("Cant open file\n");
		return -2;
	}

	int c, count = 0;
		
	while ((c = getc(f)) != EOF)
	{
		count += c == 'A';
	}

	printf("'A' count = %d\n", count);

	fclose(f);
	
	return 0;
}
