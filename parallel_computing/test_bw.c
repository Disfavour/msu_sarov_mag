#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

    int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	unsigned long N = 1e7;
    int k = 100;
    double elapsed_time;

	if (!rank)
    {
        printf("double %ld long double %ld long long %ld unsigned long %ld\n", sizeof(double), sizeof(long double), sizeof(long long), sizeof(unsigned long));
        printf("N = %ld number of experiments = %d size in bytes = %ld\n", N, k, N * sizeof(double));
    }

    double* A;

    if (rank < 2)
    {
        A = malloc(N * sizeof(double));

        for (int i = 0; i < N; i++)
        {
            A[i] = rand() / 1000;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        elapsed_time = MPI_Wtime();
    }

    for (int i = 0; i < k; i++)
    {
        if (rank == 0)
        {
            MPI_Ssend(A, N, MPI_DOUBLE, 1, 101, MPI_COMM_WORLD);
            MPI_Recv(A, N, MPI_DOUBLE, 1, 102, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank == 1)
        {
            MPI_Recv(A, N, MPI_DOUBLE, 0, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Ssend(A, N, MPI_DOUBLE, 0, 102, MPI_COMM_WORLD);
        }
    }

    if (rank == 0)
    {
        elapsed_time = MPI_Wtime() - elapsed_time;

        printf("Total time = %le\tBW = %le B/s\n", elapsed_time, (k * 2 * N*sizeof(double)) / elapsed_time);
    }

	MPI_Finalize();

	return 0;
}
