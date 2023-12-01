#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 2)
    {
        if (!rank)
        {
            printf("size != 2, size is %d\n", size);
        }
        MPI_Finalize();
        return -1;
    }

    if (argc != 2)
    {
        if (!rank)
        {
            printf("Use: %s amount_of_doubles_to_send (pass double, example: %s 1e7)\n", argv[0], argv[0]);
        }
        MPI_Finalize();
        return -2;
    }

    unsigned long N = atof(argv[1]);
    int k = 100;
    double elapsed_time;
    double* A[4];

    for (int i = 0; i < 4; i++)
    {
        A[i] = malloc(N * sizeof(double));
        for (int j = 0; j < N; j++)
        {
            A[i][j] = rand() / (rand() + 1.);
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
            MPI_Ssend(A[rand()%4], N, MPI_DOUBLE, 1, 101, MPI_COMM_WORLD);
            MPI_Recv(A[rand()%4], N, MPI_DOUBLE, 1, 102, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank == 1)
        {
            MPI_Recv(A[rand()%4], N, MPI_DOUBLE, 0, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Ssend(A[rand()%4], N, MPI_DOUBLE, 0, 102, MPI_COMM_WORLD);
        }
    }

    if (rank == 0)
    {
        elapsed_time = MPI_Wtime() - elapsed_time;
        printf("Bandwidth = 2 * K * N * sizeof(element) / time = 2 * %d * %.Le * %lu / %.3le =  %.3Le B/s\n", k, (long double)N, sizeof(double), elapsed_time, ((long double)k * 2 * N*sizeof(double)) / elapsed_time);
    }

    for (int i = 0; i < 4; i++)
    {
        free(A[i]);
    }

    MPI_Finalize();

    return 0;
}
