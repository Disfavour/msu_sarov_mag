#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main()
{
	MPI_Init(NULL, NULL);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double start_time;

	if (!rank)
		start_time = MPI_Wtime();

	int len;
	char pName[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(pName, &len);

	int size_for_int = 0;

	MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &size_for_int);

	int size_for_name = 0;

	MPI_Pack_size(MPI_MAX_PROCESSOR_NAME, MPI_CHAR, MPI_COMM_WORLD, &size_for_name);

	int size_for_pack = size_for_int + size_for_name;
	//int size_for_pack = sizeof(int) + MPI_MAX_PROCESSOR_NAME * sizeof(char);

	void* data = malloc(size_for_pack);

	int position = 0;

	MPI_Pack(&rank, 1, MPI_INT, data, size_for_pack, &position, MPI_COMM_WORLD);

	MPI_Pack(pName, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, data, size_for_pack, &position, MPI_COMM_WORLD);

	void* all;

	if (!rank)
	{
		all = malloc(size_for_pack * size);
	}

	MPI_Gather(data, size_for_pack, MPI_PACKED, all, size_for_pack, MPI_PACKED, 0, MPI_COMM_WORLD);

	if (!rank)
	{
		int position = 0;
		for (int i = 0; i < size; ++i)
		{
			int rank;
			char pName[MPI_MAX_PROCESSOR_NAME];
			MPI_Unpack(all, size_for_pack * size, &position, &rank, 1, MPI_INT, MPI_COMM_WORLD);
			MPI_Unpack(all, size_for_pack * size, &position, pName, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, MPI_COMM_WORLD);

			printf("%d %s\n", rank, pName);
		}
	}

	if (!rank)
	{
		free(all);
	}

	free(data);

	if (!rank)
	{
		printf("Elapsed time %.3lf\n", MPI_Wtime() - start_time);
	}

	MPI_Finalize();

	return 0;
}
