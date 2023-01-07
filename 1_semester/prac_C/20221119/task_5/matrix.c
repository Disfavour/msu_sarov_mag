#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"


int** get_matrix(int n, int m)
{
	int ** A = (int**) malloc(n * sizeof(int*));

	for (int i = 0; i < n; ++i)
	{
		A[i] = (int*) malloc(m * sizeof(int));
	}

	return A;
}


void free_matrix(int** A, int n)
{
	for (int i = 0; i < n; ++i)
	{
		free(A[i]);
	}

	free(A);
}


void scanf_matrix(int** A, int n, int m)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j <  m; ++j)
		{
			printf("A[%d][%d] = ", i, j);
			scanf("%d", &A[i][j]);
		}
	}
}


void printf_matrix(int** A, int n, int m)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j <  m; ++j)
		{
			printf("%d\t", A[i][j]);
		}

		putchar('\n');
	}
}


void make_rand_matrix(int** A, int n, int m, int max)
{
	srand(time(NULL));

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j <  m; ++j)
		{
			A[i][j] = rand() % max;
		}
	}
}


int get_zero_col_count(int** A, int n, int m)
{
	int ans = 0;

	for (int j = 0; j < m; ++j)
	{
		int flag = 0;

		for (int i = 0; i <  n; ++i)
		{
			if (A[i][j])
			{
				flag++;
				break;
			}
		}

		if (!flag)
			ans++;
	}

	return ans;
}


int is_min_in_column(int i, int j, int** A, int n)
{
	for (int k = 0; k < n; ++k)
	{
		if (A[i][j] > A[k][j])
		{
			return 0;
		}
	}

	return 1;
}


int is_max_in_column(int i, int j, int** A, int n)
{
	for (int k = 0; k < n; ++k)
	{
		if (A[i][j] < A[k][j])
		{
			return 0;
		}
	}

	return 1;
}


void saddle_points(int** A, int n, int m)
{
	for (int i = 0; i < n; ++i)
	{
		int
			max_val = A[i][0],
			min_val = A[i][0];

		for (int j = 1; j < m; ++j)
		{
			if (A[i][j] > max_val)
				max_val = A[i][j];
			else if (A[i][j] < min_val)
				min_val = A[i][j];
		}

		for (int j = 0; j < m; ++j)
		{
			if (A[i][j] == max_val && is_min_in_column(i, j, A, n))
			{
				printf("(%d, %d)\n", i, j);
			}

			if (A[i][j] == min_val && is_max_in_column(i, j, A, n))
			{
				printf("(%d, %d)\n", i, j);
			}
		}
	}
}


void fill_matrix_from_file(int** A, int n, int m, char* fname)
{
	FILE* f = fopen(fname, "r");

	if (!f)
	{
		printf("Cant open file for read\n");
		exit(1);
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			fscanf(f, "%d", &A[i][j]);
		}
	}

	fclose(f);
}


void write_matrix_to_file(int** A, int n, int m, char* fname)
{
	FILE* f = fopen(fname, "w");

	if (!f)
	{
		printf("Cant open file for write\n");
		exit(2);
	}

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			fprintf(f, "%d ", A[i][j]);
		}
	}

	fclose(f);
}


void fill_matrix_from_binary_file(int** A, int n, int m, char* fname)
{
	FILE* f = fopen(fname, "rb");

	if (!f)
	{
		printf("Cant open file for read\n");
		exit(1);
	}

	for (int i = 0; i < n; ++i)
	{
		fread(A[i], sizeof(int), m, f);
	}

	fclose(f);
}


void write_matrix_to_binary_file(int** A, int n, int m, char* fname)
{
	FILE* f = fopen(fname, "wb");

	if (!f)
	{
		printf("Cant open file for write\n");
		exit(2);
	}

	for (int i = 0; i < n; ++i)
	{
		fwrite(A[i], sizeof(int), m, f);
	}

	fclose(f);
}
