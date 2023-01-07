#include <stdio.h>


int main()
{
	int input_char;

	while ((input_char = getchar()) != '\n')
	{
		if (input_char == '-')
			continue;

		putchar(input_char);

		if (input_char == '&')
			putchar(input_char);
	}

	putchar('\n');

	return 0;
}