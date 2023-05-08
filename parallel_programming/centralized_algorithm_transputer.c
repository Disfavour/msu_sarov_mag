#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define n 4
#define N 16


void send_to_zero(const int from, const int rank, const MPI_Comm comm);
void send_to_target(const int target, const int rank, const MPI_Comm comm);


const char *critical_file = "critical_file.txt";


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	int size, rank, target, fd;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for (int i = 1; i < N; ++i)
	{
		send_to_zero(i, rank, MPI_COMM_WORLD);
	}

	for (int i = 1; i < N; ++i)
	{
		if (!rank)
		{
			MPI_Status status;

			MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			target = status.MPI_TAG;

			printf("0 have recieved 1st message from %d\n", target);
		}

		// MPI_Bcast can be implemented for a transputer
		MPI_Bcast(&target, 1, MPI_INT, 0, MPI_COMM_WORLD);

		send_to_target(target, rank, MPI_COMM_WORLD);

		if (rank == target)
		{
			MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, target, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			printf("%d have recieved 2nd message from 0\n", target);

			if (!access(critical_file, F_OK))
			{
				printf("File already exists\t%d/%d\n", rank, size);
				MPI_Abort(MPI_COMM_WORLD, 1);
			}
			else
			{
				fd = open(critical_file, O_CREAT, S_IRWXU);

				close(fd);

			    sleep(1+ rand() % 2);

			    remove(critical_file);
			}
		}

		send_to_zero(target, rank, MPI_COMM_WORLD);

		if (!rank)
		{
			MPI_Status status;

			MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, target, MPI_COMM_WORLD, &status);

			printf("0 have recieved 3rd message from %d\n", target);
		}
	}
	
	MPI_Finalize();

	return 0;
}


void send_to_target(const int target, const int rank, const MPI_Comm comm)
{
	const int
		col = target % 4,
		row = target / 4;

	int source = 0;

	for (int i = 0; i < col; ++i)
	{
		const int dest = source + 1;

		if (rank == source)
		{
			MPI_Send(NULL, 0, MPI_INT, dest, target, comm);
		}
		else if (rank == dest && rank != target)
		{
			MPI_Recv(NULL, 0, MPI_INT, source, target, comm, MPI_STATUS_IGNORE);
		}

		source = dest;
	}

	for (int i = 0; i < row; ++i)
	{
		const int dest = source + n;

		if (rank == source)
		{
			MPI_Send(NULL, 0, MPI_INT, dest, target, comm);
		}
		else if (rank == dest && rank != target)
		{
			MPI_Recv(NULL, 0, MPI_INT, source, target, comm, MPI_STATUS_IGNORE);
		}

		source = dest;
	}
}


void send_to_zero(const int from, const int rank, const MPI_Comm comm)
{
	const int
		col = from % 4,
		row = from / 4;

	int source = from;

	for (int i = 0; i < row; ++i)
	{
		const int dest = source - n;

		if (rank == source)
		{
			MPI_Send(NULL, 0, MPI_INT, dest, from, comm);
		}
		else if (rank == dest && rank)
		{
			MPI_Recv(NULL, 0, MPI_INT, source, from, comm, MPI_STATUS_IGNORE);
		}

		source = dest;
	}

	for (int i = 0; i < col; ++i)
	{
		const int dest = source - 1;

		if (rank == source)
		{
			MPI_Send(NULL, 0, MPI_INT, dest, from, comm);
		}
		else if (rank == dest && rank)
		{
			MPI_Recv(NULL, 0, MPI_INT, source, from, comm, MPI_STATUS_IGNORE);
		}

		source = dest;
	}
}
