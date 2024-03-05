#include "matrix.h"
#include <stdio.h>


int main()
{
    matrix A = create_matrix(2, 3);
    init_matrix(A);
    print_matrix(A);

    matrix B = create_matrix(3, 2);
    init_matrix(B);
    print_matrix(B);

    printf("A+A\n");
    print_matrix(add(A, A));

    printf("A*B\n");
    print_matrix(multiply(A, B));

    return 0;
}
