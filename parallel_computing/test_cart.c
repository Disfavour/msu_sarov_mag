#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>


struct procinfo
{
    int grid_coords[2];
    int left, right, up, down;
    int rank;
};


int main(int argc, char** argv)
{
    struct procinfo info;

    MPI_Comm Grid_Comm;
    MPI_Status status;
    int size, rank;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int dims[] = {0, 0};
    MPI_Dims_create(size, 2, dims);

    int periods[] = {0, 0};

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &Grid_Comm);
    MPI_Comm_rank(Grid_Comm, &info.rank);
    MPI_Cart_coords(Grid_Comm, info.rank, 2, info.grid_coords);

    printf("%d/%d -> %d/%d (%d, %d) dims (%d, %d)\n", rank, size, info.rank, size, info.grid_coords[0], info.grid_coords[1], dims[0], dims[1]);

    MPI_Cart_shift(Grid_Comm, 0, 1, &info.left, &info.right);
    MPI_Cart_shift(Grid_Comm, 1, 1, &info.up, &info.down);

    sleep(1);
    printf("%d/%d left %d right %d up %d down %d\n", info.rank, size, info.left, info.right, info.up, info.down);

    MPI_Finalize();

    return 0;
}
