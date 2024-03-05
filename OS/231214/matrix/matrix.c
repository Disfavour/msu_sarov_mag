#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"


void print_matrix(matrix A)
{
    for (int i = 0; i < A.n; i++)
    {
        for (int j = 0; j < A.m; j++)
        {
            printf("%lf, ", A.data[i][j]);
        }
        printf("\n");
    }
}

void init_matrix(matrix A)
{
    for (int i = 0; i < A.n; i++)
    {
        for (int j = 0; j < A.m; j++)
        {
            printf("A[%d][%d] = ", i, j);
            if (scanf("%lf", &A.data[i][j]) != 1)
                fprintf(stderr, "scanf");
        }
    }
}

matrix create_matrix(int n, int m)
{
    matrix A;
    A.n = n;
    A.m = m;
    A.data = (double**)malloc(A.n * sizeof(double*));

    for (int i = 0; i < A.n; i++)
    {
        A.data[i] = (double*)malloc(A.m * sizeof(double));
    }
    return A;
}

void delete_matrix(matrix* A)
{
    for (int i = 0; i < A->n; i++)
    {
        free(A->data[i]);
    }

    free(A->data);

    A->n = 0;
    A->m = 0;
}

matrix add(matrix A, matrix B)
{
    matrix C = create_matrix(A.n, A.m);

    for (int i = 0; i < C.n; i++)
    {
        for (int j = 0; j < C.m; j++)
        {
            C.data[i][j] = A.data[i][j] + B.data[i][j];
        }
    }
    return C;
}

matrix multiply(matrix A, matrix B)
{
    matrix C = create_matrix(A.n, B.m);

    for (int i = 0; i < A.n; i++)
    {
        for (int j = 0; j < B.m; j++)
        {
            C.data[i][j] = 0;
            for (int k = 0; k < A.m; k++)
            {
                C.data[i][j] += A.data[i][k] * B.data[k][j];
            }
        }
    }
    return C;
}
