#include "matrix.h"
#include <stdio.h>


int main()
{
	int
		n = 2,
		m = 8;

	int** A = get_matrix(n, m);

	make_rand_matrix(A, n, m, 2);

	printf_matrix(A, n, m);

	printf("\nNumber of zero columns = %d\n", get_zero_col_count(A, n, m));

	printf("\nSaddle points:\n");
	saddle_points(A, n, m);

	free_matrix(A, n);

	return 0;
}