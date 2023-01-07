#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct MES
{
	int rank;
	char pName[MPI_MAX_PROCESSOR_NAME];
} MES;


int main()
{
	MPI_Init(NULL, NULL);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double start_time;

	if (!rank)
		start_time = MPI_Wtime();

	int blocklens[] = {1, MPI_MAX_PROCESSOR_NAME};
	MPI_Datatype types[] = {MPI_INT, MPI_CHAR};
	MPI_Aint displs[] = {0, blocklens[0] * sizeof(int)};

	MPI_Datatype newtype;

	MPI_Type_create_struct(2, blocklens, displs, types, &newtype);

	MPI_Type_commit(&newtype);

	MES data;
	data.rank = rank;
	int len;
	MPI_Get_processor_name(data.pName, &len);

	MES* all;

	if (!rank)
	{
		all = malloc(size * sizeof(MES));
	}

	MPI_Gather(&data, 1, newtype, all, 1, newtype, 0, MPI_COMM_WORLD);

	if (!rank)
	{
		for (int i = 0; i < size; ++i)
		{
			printf("%d %s\n", all[i].rank, all[i].pName);
		}
	}

	if (!rank)
	{
		free(all);
	}

	MPI_Type_free(&newtype);

	if (!rank)
	{
		printf("Elapsed time %.3lf\n", MPI_Wtime() - start_time);
	}

	MPI_Finalize();

	return 0;
}
