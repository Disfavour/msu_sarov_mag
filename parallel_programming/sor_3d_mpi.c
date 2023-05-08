#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define  Max(a,b) ((a)>(b)?(a):(b))


#define N 130
#define Nparts 32 	// Number of blocks per process. Must be more or equal than the number of processes
#define maxeps 1e-8


int get_ksize_without_overlap(const int rank, const int size);
int get_ksize(const int rank, const int size);
int get_kreal_min(const int rank, const int size);

double** get_memory(const int rank, const int size);
void free_memory(const int rank, const int size, double **A);

void init(const int rank, const int size, double **A);
double verify(const int rank, const int size, double **A);


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double start_time;

	if (!rank)
	{
		start_time = MPI_Wtime();
	}

	// A three-dimensional array is simulated as two-dimensional
	// to send and receive two-dimensional arrays in 1 mpi operation
	double **A = get_memory(rank, size);

	init(rank, size, A);

	for (int it = 1; it <= 100; it++)
	{
		const int
			ksize = get_ksize(rank, size),	// 0 proc gets more ksize if k can't be divided equally
			top = (rank - 1 < 0) ? (MPI_PROC_NULL) : (rank - 1),
			bot = (rank + 1 >= size) ? (MPI_PROC_NULL) : (rank + 1);

		double eps = 0;

		// update shadow edges
		MPI_Request request[4];

		MPI_Irecv(A[0], N*N, MPI_DOUBLE, top, 102, MPI_COMM_WORLD, &request[0]);

		MPI_Irecv(A[ksize-1], N*N, MPI_DOUBLE, bot, 101, MPI_COMM_WORLD, &request[1]);

		MPI_Isend(A[1], N*N, MPI_DOUBLE, top, 101, MPI_COMM_WORLD, &request[2]);

		MPI_Isend(A[ksize-2], N*N, MPI_DOUBLE, bot, 102, MPI_COMM_WORLD, &request[3]);

		MPI_Waitall(4, request, MPI_STATUS_IGNORE);

		// relax
		const int nrows_per_block = (N - 2) / Nparts;

		for (int p = 0; p < Nparts; ++p)
		{
			const int
				istart = 1 + p * nrows_per_block,
				imax = (p == Nparts-1) ? (N-1) : (1 + (p+1)*nrows_per_block),
				nrows_in_block = imax - istart;	// last block is bigger if nrows can't be divided equally

			MPI_Recv(&A[0][istart*N], nrows_in_block*N, MPI_DOUBLE, top, 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			for (int k = 1; k < ksize - 1; ++k)
			{
				for (int i = istart; i < imax; ++i)
				{
					for (int j = 1; j < N - 1; ++j)
					{
						double e = A[k][i*N+j];

						A[k][i*N+j]=(A[k-1][i*N+j]+A[k+1][i*N+j]+A[k][(i-1)*N+j]+A[k][(i+1)*N+j]+A[k][i*N+j-1]+A[k][i*N+j+1]) / 6;

						eps = Max(eps, fabs(e - A[k][i*N+j]));
					}
				}
			}

			MPI_Send(&A[ksize-2][istart*N], nrows_in_block*N, MPI_DOUBLE, bot, 111, MPI_COMM_WORLD);
		}

		MPI_Allreduce(MPI_IN_PLACE, &eps, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

		if (!rank)
		{
			printf("it=%4i   eps=%f\n", it, eps);
		}

		if (eps < maxeps) 
		{
			break;
		}
	}

	double s = verify(rank, size, A);

	free_memory(rank, size, A);

	double total_s = 0;

	MPI_Reduce(&s, &total_s, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (!rank)
	{
		printf("S = %f\n", total_s);
		printf("Elapsed time = %.3lf\n", MPI_Wtime() - start_time);
	}

	MPI_Finalize();

	return 0;
}


int get_ksize_without_overlap(const int rank, const int size)
{
	return (rank) ? ((N-2)/size) : ((N-2)/size + (N-2)%size);
}


int get_ksize(const int rank, const int size)
{
	return get_ksize_without_overlap(rank, size) + 2;
}


int get_kreal_min(const int rank, const int size)
{
	return (rank) ? (N-2 - (size-rank)*get_ksize_without_overlap(rank, size)) : (0);
}


double** get_memory(const int rank, const int size)
{
	const int ksize = get_ksize(rank, size);

	double **A = malloc(ksize * sizeof(double*));

	for (int k = 0; k < ksize; ++k)
	{
		A[k] = malloc(N*N * sizeof(double));
	}

	return A;
}


void free_memory(const int rank, const int size, double **A)
{
	const int ksize = get_ksize(rank, size);

	for (int k = 0; k < ksize; ++k)
	{
		free(A[k]);
	}
	free(A);
}


void init(const int rank, const int size, double **A)
{
	const int
		ksize = get_ksize(rank, size),
		kreal_min = get_kreal_min(rank, size);

	for (int k = 0; k < ksize; ++k)
	{
		const int kreal = kreal_min + k;

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				if (kreal==0 || kreal==N-1 || i==0 || i==N-1 || j==0 || j==N-1)
				{
					A[k][i*N+j] = 0;
				}
				else
				{
					A[k][i*N+j] = 4 + kreal + i + j;
				}
			}
		}
	}
}


double verify(const int rank, const int size, double **A)
{
	double s = 0;

	const int
		ksize = get_ksize(rank, size),
		kreal_min = get_kreal_min(rank, size);

	for (int k = 1; k < ksize - 1; ++k)
	{
		const int kreal = kreal_min + k;

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				s += A[k][i*N+j] * (kreal+1) * (i+1) * (j+1);
			}
		}
	}
	s /= N*N*N;

	return s;
}
