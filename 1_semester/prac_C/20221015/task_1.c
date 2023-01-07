#include <stdio.h>


int main()
{
	int input_num;

	if (scanf("%d", &input_num))
	{
		while (input_num && input_num % 3 == 0)
		{
			input_num /= 3;
		}

		if (input_num == 1)
			printf("Yes\n");
		else
			printf("No\n");
	}

	return 0;
}