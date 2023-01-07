#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define N (1024 * 256)
#define TAG 13423


double complex_calculations(double n)
{
	for (int i = 0; i < 1000; ++i)
	{
		if (i % 2)
		{
			n = pow(n, 0.01);
		}
		else
		{
			n = pow(n, 100);
		}
	}
	return n; 
}


int main()
{
	MPI_Init(NULL, NULL);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double start_time;

	if (!rank)
		start_time = MPI_Wtime();

	double ans = 0;

	int part = rank ? N / size : N - N / size * (size - 1);

	double v1[part], v2[part];

	srand(TAG);

	for (int i = 0; i < part; ++i)
	{
		v1[i] = complex_calculations(rand() % 10000 / 10000.);
		v2[i] = complex_calculations(rand() % 10000 / 10000.);
	}

	for (int i = 0; i < part; ++i)
	{
		ans += v1[i] * v2[i];
	}

	if (rank)
	{
		MPI_Send(&ans, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
	}
	else
	{
		for (int i = 1; i < size; ++i)
		{
			double part_ans;
			MPI_Status status;

			MPI_Recv(&part_ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
			ans += part_ans;
		}

		printf("Answer = %lf\n", ans);
	}

	if (!rank)
	{
		printf("Elapsed time %.3lf\n", MPI_Wtime() - start_time);
	}

	MPI_Finalize();

	return 0;
}
