#include "matrix.h"
#include <stdio.h>


int main()
{
	int
		n = 5,
		m = 6;

	int** A = get_matrix(n, m);

	make_rand_matrix(A, n, m, 10000);

	printf("\nInitial matrix\n");
	printf_matrix(A, n, m);

	write_matrix_to_file(A, n, m, "matrix.txt");
	printf("\nMatrix is written to the file \"matrix.txt\"\n");

	fill_matrix_from_file(A, n, m, "matrix.txt");
	printf("\nMatrix from file \"matrix.txt\"\n");
	printf_matrix(A, n, m);

	write_matrix_to_binary_file(A, n, m, "matrix.bin");
	printf("\nMatrix is written to the file \"matrix.bin\"\n");

	fill_matrix_from_binary_file(A, n, m, "matrix.bin");
	printf("\nMatrix from file \"matrix.bin\"\n");
	printf_matrix(A, n, m);

	free_matrix(A, n);

	return 0;
}
