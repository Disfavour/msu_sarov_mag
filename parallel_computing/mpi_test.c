#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int main()
{
	MPI_Init(NULL, NULL);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int len;
	char pName[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(pName, &len);

    #pragma omp parallel
	{
        #pragma omp master
        printf("%d/%d %d %s\n", rank, size, omp_get_num_threads(), pName);
    }

    MPI_Finalize();

    return 0;
}