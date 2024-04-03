#include <fstream>
#include <mpi.h>
#include <cstring>
#include <math.h>
using namespace std;


int f(int* data, int i, int j, int n)
{
	int state = data[i*(n+2)+j];
	int s = -state;
	for( int ii = i - 1; ii <= i + 1; ii ++ )
		for( int jj = j - 1; jj <= j + 1; jj ++ )
			s += data[ii*(n+2)+jj];
	if( state==0 && s==3 )
		return 1;
	if( state==1 && (s<2 || s>3) ) 
		return 0;
	return state;
}

void update_data(int n, int* data, int* temp)
{
	for( int i=1; i<=n; i++ )
		for( int j=1; j<=n; j++ )
			temp[i*(n+2)+j] = f(data, i, j, n);
}

void init(int n, int* data)
{
	for( int i=0; i<(n+2)*(n+2); i++ )
		data[i] = 0;
	int n0 = 1+n/2;
	int m0 = 1+n/2;
	data[(n0-1)*(n+2)+m0] = 1;
	data[n0*(n+2)+m0+1] = 1;
	for( int i=0; i<3; i++ )
		data[(n0+1)*(n+2)+m0+i-1] = 1;
}

void run_life(int n, int T)
{
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm comm;

	int dims[] = {0, 0};
    MPI_Dims_create(size, 2, dims);

	int periods[2] = {true, true};
	int reorder = true;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &comm);
	
    MPI_Comm_rank(comm, &rank);

    int left, right, top, bot;
    MPI_Cart_shift(comm, 1, 1, &left, &right);
    MPI_Cart_shift(comm, 0, 1, &top, &bot);
	// left to right and top to bottom
	// (3, 2) - (0, 1) - (rows, columns)
	// (0, 0) (0, 1)
	// (1, 0) (1, 1)
	// (2, 0) (2, 1)

	const int p = dims[0]; //sqrt(size);
	const int N = n*p;

	MPI_Datatype
		global_matrix_not_resized,
		global_matrix,
		matrix_not_resized,
		matrix,
		row,
		column;

	int *data, *temp, *global_data;

	if (rank == 0)
	{
		global_data = new int[N*N];

		init(N-2, global_data);

		int sizes[2]    = {N, N};
		int subsizes[2] = {n, n};
		int starts[2]   = {0, 0};

		MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &global_matrix_not_resized);  
		MPI_Type_create_resized(global_matrix_not_resized, 0, n*sizeof(int), &global_matrix); // extent = кол-во столбцов матрицы
		MPI_Type_commit(&global_matrix);
	}

	data = new int[(n+2)*(n+2)] {};
	temp = new int[(n+2)*(n+2)] {};

	int sizes[2]    = {n+2, n+2};
	int subsizes[2] = {n, n};
	int starts[2]   = {1, 1};

	MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &matrix);
	MPI_Type_commit(&matrix);

    MPI_Type_contiguous(n+2, MPI_INT, &row);
	MPI_Type_commit(&row);

    MPI_Type_vector(n+2, 1, n+2, MPI_INT, &column);
    MPI_Type_commit(&column);

	int *counts = new int[size];
	int *displs = new int[size];

	int counter = 0;
	for (int i = 0; i < p; i++)
	{
		for (int j = 0; j < p; j++)
		{
			counts[counter] = 1;
			displs[counter] = i*N+j; // i*(n*N) + j если отступ в int'ах
			counter++;
		}
	}

	MPI_Scatterv(global_data, counts, displs, global_matrix, data, 1, matrix, 0, MPI_COMM_WORLD);

	double elapsed_time = 0;

	if (!rank)
	{
		elapsed_time = MPI_Wtime();
	}
	
	for( int t = 0; t < T; t++ )
	{
		MPI_Sendrecv(data+n, 1, column, right, 123, data, 1, column, left, 123, comm, MPI_STATUS_IGNORE);
		MPI_Sendrecv(data+1, 1, column, left, 124, data+n+1, 1, column, right, 124, comm, MPI_STATUS_IGNORE);

		MPI_Sendrecv(data+n*(n+2), 1, row, bot, 125, data, 1, row, top, 125, comm, MPI_STATUS_IGNORE);
		MPI_Sendrecv(data+1*(n+2), 1, row, top, 126, data+(n+1)*(n+2), 1, row, bot, 126, comm, MPI_STATUS_IGNORE);

		update_data(n, data, temp);
		swap(data, temp);
	}

	MPI_Gatherv(data, 1, matrix, global_data, counts, displs, global_matrix, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		elapsed_time = MPI_Wtime() - elapsed_time;

		ofstream f("output.dat");
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
				f << global_data[i*N+j];
			f << endl;
		}
		f.close();

		ofstream f2("stat.dat");
		f2 << n << " " << T << " " << p << " " << elapsed_time << endl;
		f2.close();

		ofstream f3("data_for_plots.dat", ios::app);
		f3 << N << " " << size << " " << elapsed_time << endl;
		f3.close();
	}

	delete[] counts;
	delete[] displs;

	if (rank == 0)
	{
		delete[] global_data;
		MPI_Type_free(&global_matrix);
	}

	MPI_Type_free(&row);
    MPI_Type_free(&column);
	MPI_Type_free(&matrix);

	delete[] data;
	delete[] temp;
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	
	int n = atoi(argv[1]);
	int T = atoi(argv[2]);

	run_life(n, T);

	MPI_Finalize();

	return 0;
}
