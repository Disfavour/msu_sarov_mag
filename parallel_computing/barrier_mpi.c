#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>


#define TAG 12345


int get_layers(int size)
{
	const float x = log2(size);
	return (x - (int)x == 0) ? x : ceil(x);
}


void barrier(int rank, int size)
{
	const int layers = get_layers(size);

	for (int i = 1; i <= layers; ++i)
	{
		const int
			k = 1 << i,
			offset = k >> 1;

		for (int j = 0; j*k < size; ++j)
		{
			const int
				receiver = j*k,
				sender = (receiver + offset < size) ? (receiver + offset) : (MPI_PROC_NULL);

			if (rank == receiver)
			{
				MPI_Recv(NULL, 0, MPI_CHAR, sender, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
			else if (rank == sender)
			{
				MPI_Send(NULL, 0, MPI_CHAR, receiver, TAG, MPI_COMM_WORLD);
			}
		}
	}

	for (int i = layers; i >= 1; --i)
	{
		const int
			k = 1 << i,
			offset = k >> 1;

		for (int j = 0; j*k < size; ++j)
		{
			const int
				sender = j*k,
				receiver = (sender + offset < size) ? (sender + offset) : (MPI_PROC_NULL);

			if (rank == sender)
			{
				MPI_Send(NULL, 0, MPI_CHAR, receiver, TAG, MPI_COMM_WORLD);
			}
			else if (rank == receiver)
			{
				MPI_Recv(NULL, 0, MPI_CHAR, sender, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}
		}
	}
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 1)
	{
		sleep(2);
	}
	else if (rank && !(rank % 3))
	{
		sleep(4);
	}

	printf("Start of the barrier %d/%d\n", rank, size);

	barrier(rank, size);

	printf("End of the barrier %d/%d\n", rank, size);

	MPI_Finalize();

	return 0;
}
