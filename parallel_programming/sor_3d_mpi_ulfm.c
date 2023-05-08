#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <mpi-ext.h>


#define  Max(a,b) ((a)>(b)?(a):(b))


#define N 130
#define Nparts 32 	// Number of blocks per process. Must be more or equal than the number of processes
#define maxeps 1e-8

#define filename "file.bin"
#define ITER 10 	// iteration % ITER == 0 => control point

#define NERRORS 2 	// Maximum number of processes killed
#define CHANCE 5	// The probability of killing the process at each iteration (%)


int get_ksize_without_overlap(const int rank, const int size);
int get_ksize(const int rank, const int size);
int get_kreal_min(const int rank, const int size);

double** get_memory(const int rank, const int size);
void free_memory(const int rank, const int size, double **A);

void init(const int rank, const int size, double **A);
double verify(const int rank, const int size, double **A);

void write_data(const int rank, const int size, double **A, MPI_Comm comm);
void read_data(const int rank, const int size, double **A, MPI_Comm comm);

static void verbose_errhandler(MPI_Comm* pcomm, int* perr, ...);


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	int size, rank;

	MPI_Comm comm;

	MPI_Comm_dup(MPI_COMM_WORLD, &comm);

	MPI_Comm_size(comm, &size);

	MPI_Comm_rank(comm, &rank);

	MPI_Errhandler errh;

	MPI_Comm_create_errhandler(verbose_errhandler, &errh);

    MPI_Comm_set_errhandler(comm, errh);

	int rc = MPI_SUCCESS;

	int nerrors = NERRORS;

	double start_time;

	start_time = MPI_Wtime();

	// A three-dimensional array is simulated as two-dimensional
	// to send and receive two-dimensional arrays in 1 mpi operation
	double **A = get_memory(rank, size);

	init(rank, size, A);

	write_data(rank, size, A, comm);

	for (int it = 1; it <= 100; it++)
	{
		if (rc != MPI_SUCCESS)
		{
			read_data(rank, size, A, comm);
		}

		const int
			ksize = get_ksize(rank, size),	// 0 proc gets more ksize if k can't be divided equally
			top = (rank - 1 < 0) ? (MPI_PROC_NULL) : (rank - 1),
			bot = (rank + 1 >= size) ? (MPI_PROC_NULL) : (rank + 1);

		double eps = 0;

		// update shadow edges
		MPI_Request request[4];

		MPI_Irecv(A[0], N*N, MPI_DOUBLE, top, 102, comm, &request[0]);

		MPI_Irecv(A[ksize-1], N*N, MPI_DOUBLE, bot, 101, comm, &request[1]);

		MPI_Isend(A[1], N*N, MPI_DOUBLE, top, 101, comm, &request[2]);

		MPI_Isend(A[ksize-2], N*N, MPI_DOUBLE, bot, 102, comm, &request[3]);

		MPI_Waitall(4, request, MPI_STATUS_IGNORE);

		// relax
		const int nrows_per_block = (N - 2) / Nparts;

		for (int p = 0; p < Nparts; ++p)
		{
			const int
				istart = 1 + p * nrows_per_block,
				imax = (p == Nparts-1) ? (N-1) : (1 + (p+1)*nrows_per_block),
				nrows_in_block = imax - istart;	// last block is bigger if nrows can't be divided equally

			MPI_Recv(&A[0][istart*N], nrows_in_block*N, MPI_DOUBLE, top, 111, comm, MPI_STATUS_IGNORE);

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

			MPI_Send(&A[ksize-2][istart*N], nrows_in_block*N, MPI_DOUBLE, bot, 111, comm);
		}

		if (nerrors > 0 && (rand() % 100 < CHANCE))
		{
			if (rank == size-1)
			{
				raise(SIGKILL);
			}

			nerrors--;
		}

		rc = MPI_Allreduce(MPI_IN_PLACE, &eps, 1, MPI_DOUBLE, MPI_MAX, comm);

		if (rc != MPI_SUCCESS)
	    {
	    	// A dead process can't free memory
	    	free_memory(rank, size, A);

	    	MPI_Comm new_comm;

		    MPIX_Comm_shrink(comm, &new_comm);

		    MPIX_Comm_revoke(comm);
		    MPI_Comm_free(&comm);

		    comm = new_comm;

		    MPI_Errhandler errh;
		    MPI_Comm_create_errhandler(verbose_errhandler, &errh);
		    MPI_Comm_set_errhandler(comm, errh);

		    MPI_Comm_size(comm, &size);

			MPI_Comm_rank(comm, &rank);

			A = get_memory(rank, size);

			it = (it-1) / ITER * ITER;

			continue;
	    }

	    if (!(it % ITER))
		{
			write_data(rank, size, A, comm);
		}

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

	MPI_Reduce(&s, &total_s, 1, MPI_DOUBLE, MPI_SUM, 0, comm);

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


void write_data(const int rank, const int size, double **A, MPI_Comm comm)
{
	MPI_File fh;

	MPI_File_open(comm, filename, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

	const int ksize = get_ksize(rank, size);

	int offset = get_kreal_min(rank, size) * N*N * sizeof(double);

	for (int k = 1; k < ksize - 1; ++k)
	{
		MPI_File_write_at(fh, offset, A[k], N*N, MPI_DOUBLE, MPI_STATUS_IGNORE);

		offset += N*N * sizeof(double);
	}

	MPI_File_close(&fh);
}


void read_data(const int rank, const int size, double **A, MPI_Comm comm)
{
	MPI_File fh;

	MPI_File_open(comm, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

	const int ksize = get_ksize(rank, size);

	int offset = get_kreal_min(rank, size) * N*N * sizeof(double);

	for (int k = 1; k < ksize - 1; ++k)
	{
		MPI_File_read_at(fh, offset, A[k], N*N, MPI_DOUBLE, MPI_STATUS_IGNORE);

		offset += N*N * sizeof(double);
	}

	MPI_File_close(&fh);
}


static void verbose_errhandler(MPI_Comm* pcomm, int* perr, ...) {
    MPI_Comm comm = *pcomm;
    int err = *perr;
    char errstr[MPI_MAX_ERROR_STRING];
    int i, rank, size, nf, len, eclass;
    MPI_Group group_c, group_f;
    int *ranks_gc, *ranks_gf;

    MPI_Error_class(err, &eclass);
    if( MPIX_ERR_PROC_FAILED != eclass ) {
        MPI_Abort(comm, err);
    }
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    /* We use a combination of 'ack/get_acked' to obtain the list of failed processes.     */
    MPIX_Comm_failure_ack(comm);
    MPIX_Comm_failure_get_acked(comm, &group_f);
    MPI_Group_size(group_f, &nf);
    MPI_Error_string(err, errstr, &len);
    printf("Rank %d / %d: Notified of error %s. %d found dead: { ", rank, size, errstr, nf);
    /* We use 'translate_ranks' to obtain the ranks of failed procs  in 'comm‘ communicator  */
    ranks_gf = (int*)malloc(nf * sizeof(int));
    ranks_gc = (int*)malloc(nf * sizeof(int));
    MPI_Comm_group(comm, &group_c);
    for(i = 0; i < nf; i++)
        ranks_gf[i] = i;
    MPI_Group_translate_ranks(group_f, nf, ranks_gf,
                              group_c, ranks_gc);
    for(i = 0; i < nf; i++)
        printf("%d ", ranks_gc[i]);
    printf("}\n");
    free(ranks_gf); free(ranks_gc);
}
