#include <stdio.h>


int main()
{
	int input_num;

	if (scanf("%d", &input_num))
	{
		int ans = 1;

		while (ans <= input_num)
		{
			ans *= 2;
		}

		printf("Answer = %d\n", ans);
	}

	return 0;
}