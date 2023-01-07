#include <stdio.h>


// Поиск строки максимальной длины в файле
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("argc != 2\n");
		return -1;
	}

	FILE* f = fopen(argv[1], "r");

	if (!f)
	{
		printf("Cant open file\n");
		return -2;
	}

	int max_len = 0;
	long marker = ftell(f);

	int c, curlen = 0, cur_marker = ftell(f);

	while ((c = fgetc(f)) != EOF)
	{
		if (c == '\n')
		{
			if (curlen > max_len)
			{
				max_len = curlen;
				marker = cur_marker;
			}
			
			curlen = 0;
			cur_marker = ftell(f);
		}
		else	
			curlen++;
	}

	fseek(f, marker, SEEK_SET);
	char str[max_len+1];
	
	fgets(str, max_len + 1, f);

	fputs(str, stdout);
	putchar('\n');

	return 0;
}
