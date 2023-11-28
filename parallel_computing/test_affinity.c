#define _GNU_SOURCE
#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int len;
	char pName[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(pName, &len);

    cpu_set_t mask;
    CPU_ZERO(&mask);
    sched_getaffinity(getpid(), sizeof(cpu_set_t), &mask);
    char res[100];
    res[0] = '\0';
    for (int i = 0; i < CPU_SETSIZE; ++i)
    {
        if (CPU_ISSET(i, &mask))
        {
            char tmp[10];
            sprintf(tmp,"%d", i);
            strncat(res, tmp, strlen(tmp));
            strncat(res, ", ", 1);
        }
    }
    printf("mpi %d/%d %s %s\n", rank, size, res, pName);


    // CPU_ZERO(&mask);
    // for (int i = 0; i < CPU_SETSIZE; i++)
    // {
    //     CPU_SET(i, &mask);
    // }
    // sched_setaffinity(0, sizeof(cpu_set_t), &mask);

    MPI_Barrier(MPI_COMM_WORLD);

    #pragma omp parallel
	{
        int
            tr = omp_get_thread_num(),
            ts = omp_get_num_threads();
        
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(tr, &mask);
        sched_setaffinity(getpid(), sizeof(cpu_set_t), &mask);

        //cpu_set_t mask;
        CPU_ZERO(&mask);
        sched_getaffinity(getpid(), sizeof(cpu_set_t), &mask);
        char res[100];
        res[0] = '\0';
        for (int i = 0; i < CPU_SETSIZE; ++i)
        {
            if (CPU_ISSET(i, &mask))
            {
                char tmp[10];
                sprintf(tmp,"%d", i);
                strncat(res, tmp, strlen(tmp));
                strncat(res, ", ", 1);
            }
        }
        printf("mpi %d/%d omp %d/%d %s %s\n", rank, size, tr, ts, res, pName);
    }

    MPI_Finalize();

    return 0;
}