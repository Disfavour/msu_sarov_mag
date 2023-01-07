#include <stdio.h>


// Посимвольное копирование файла
int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("argc != 3\n");
		return -1;
	}
	
	FILE* f_in = fopen(argv[1], "r");
	
	if (!f_in)
	{
		printf("Cant open first file\n");
		return -2;
	}

	FILE* f_out = fopen(argv[2], "w");

	if (!f_out)
	{
		printf("Cant open second file\n");
		return -3;
	}

	int c;

	while ((c = fgetc(f_in)) != EOF)
		fputc(c, f_out);

	fclose(f_in);
	fclose(f_out);
	
	return 0;
}
