#include <stdio.h>


void my_strcmp(char str1[], char str2[])
{
	int i = 0;

	while (str1[i])
	{
		if (str1[i] != str2[i])
		{
			printf("False\n");
			return;
		}

		i++;
	}

	if (str1[i] != str2[i])
		printf("False\n");
	else
		printf("True\n");
}
