#include <stdio.h>

void print_bin(int num)
{
	int a[32];

	for (int i = 0; i < 32; i++)
	{
		a[i] = 0;
	}

	for (int i = 31; num; i--)
	{
		a[i] = num % 2;
		num /= 2;
	}

	for (int i = 0; i < 32; i++)
	{
		if (i % 8 == 0 && i > 0)
		{
			printf(" ");
		}

		printf("%d", a[i]);
	}
	printf("\n");
}

void task5()

{
	// int i = (1 || 2) % (1 | 2); i = 1 % 0
	// printf (”i = %d\n”, i);

	int x = 03, y = 02, z = 01;

	print_bin(x);
	print_bin(y);
	print_bin(z);

	printf("\n");

	print_bin(x | y & -z);
	printf("%d\n", x | y & -z);

	printf("\n");

	print_bin(x ^ y & -z);
	printf("%d\n", x ^ y & -z);

	printf("\n");

	print_bin(x & y && z);
	printf("%d\n", x & y && z);

	printf("\n");

	print_bin(x<<3);
	printf("%d\n", x<<3);

	printf("\n");
}

void quest (int*a, int n)
{

	// *a = a[0]
	// n=*a, *a=a[1], a[*a>n]=n ==>>
	/*
	if (a[1] > a[0])
	a[0], a[1] = a[1], a[0]

	else if (a[1] <= a[0])
	a[0], a[1] = a[0], a[1]

	*/

	//n-- > 1 ? quest(a,n), quest(a+1,n), n=*a, *a=a[1], a[*a>n]=n : 0;

	if (n > 1)
	{
		for (int i = 0; i < n; i++)
		{
			printf("%d", a[i]);
		}
		printf("\n");

		quest(a, n - 1);
		quest(a + 1, n - 1);

		if (a[0] < a[1])

		{
			int tmp = a[0];
			a[0] = a[1];
			a[1] = tmp;

			printf("swap ");
			for (int i = 0; i < n; i++)
			{
				printf("%d", a[i]);
			}
			printf("\n");
		}
	}
}

int main()
{
	// 1 (a / b) * b + a % b = a
	printf("Result = %d \n",-5 % -3);

	// 2
	printf("short %ld, int %ld, long %ld, long long %ld, float %ld, double %ld, long double %ld\n",
	sizeof(short), sizeof(int), sizeof(long), sizeof(long long), sizeof(float), sizeof(double),
	sizeof(long double));

	// 3
	unsigned y;
	char
	c1 = 'q',
	c2 = 'w',
	c3 = 'e',
	c4 = 'r';

	print_bin(c1);
	print_bin(c2);
	print_bin(c3);
	print_bin(c4);

	y = ((unsigned) c1 << 3 * 8) + ((unsigned) c2 << 2 * 8) + ((unsigned) c3 << 1 * 8) +
	(unsigned) c4;

	print_bin(y);

	// 4
	int k = 0;
	int x = 153;

	printf("x = %d, ", x);

	while (x)
	{
		k += x % 10;
		x /= 10;
	}

	printf("k = %d\n", k);

	// 5
	task5();

	// quest
	const int n = 4;
	int v[]={1, 2, 3, 4};
	/*

	for (int i = 0; i < n; i++)
	{
	v[i] = i;
	}
	*/

	quest(v, n);

	for (int i = 0; i < n; i++)
	{
		printf("%d", v[i]);
	}
	printf("\n");

	return 0;
}