#include <stdio.h>


int main()
{
	int input_char, prev_char = ' ';

	while ((input_char = getchar()) != '\n')
	{
		if (input_char == '+' && prev_char == '+')
			continue;

		putchar(input_char);

		prev_char = input_char;
	}

	putchar('\n');

	return 0;
}