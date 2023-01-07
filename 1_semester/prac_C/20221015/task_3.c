#include <stdio.h>


int main()
{
	int input_num;

	if (scanf("%d", &input_num))
	{
		int
			prev = 0,
			cur = 1,
			next;

		if (input_num == 1)
			printf("Answer = %d\n", cur);

		else if (input_num > 1)
		{
			printf("Answer = %d", cur);

			for (int i = 0; i < input_num - 1; i++)
			{
				next = prev + cur;

				printf(" %d", next);

				prev = cur;
				cur = next;
			}

			printf("\n");
		}

	}

	return 0;
}