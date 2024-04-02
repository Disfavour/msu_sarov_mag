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

void init(int n, int* data, int* temp)
{
	for( int i=0; i<(n+2)*(n+2); i++ )
		data[i] = temp[i] = 0;
	int n0 = 1+n/2;
	int m0 = 1+n/2;
	data[(n0-1)*(n+2)+m0] = 1;
	data[n0*(n+2)+m0+1] = 1;
	for( int i=0; i<3; i++ )
		data[(n0+1)*(n+2)+m0+i-1] = 1;
}

void setup_boundaries(int n, int* data)
{
	for( int i=0; i<n+2; i++ )
	{
		data[i*(n+2)+0] = data[i*(n+2)+n];
		data[i*(n+2)+n+1] = data[i*(n+2)+1];
	}
	for( int j=0; j<n+2; j++ )
	{
		data[0*(n+2)+j] = data[n*(n+2)+j];
		data[(n+1)*(n+2)+j] = data[1*(n+2)+j];
	}
}

void run_life(int n, int T)
{
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	const int px = sqrt(size);
	const int N = n*px;

	MPI_Datatype
		global_matrix_not_resized,
		global_matrix,
		matrix_not_resized,
		matrix;

	int *data, *temp, *global_data, *global_temp;

	if (rank == 0)
	{
		global_data = new int[N*N];
		global_temp = new int[N*N];

		init(N-2, global_data, global_temp);

		int sizes[2]    = {N, N};
		int subsizes[2] = {n, n};
		int starts[2]   = {0, 0};

		MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &global_matrix_not_resized);  
		MPI_Type_create_resized(global_matrix_not_resized, 0, n*sizeof(int), &global_matrix); // extent = кол-во столбцов матрицы
		MPI_Type_commit(&global_matrix);
		//MPI_Type_vector(n, n, N+2, MPI_INT, &matrix);
	}

	data = new int[(n+2)*(n+2)];
	temp = new int[(n+2)*(n+2)];

	int sizes[2]    = {n+2, n+2};
	int subsizes[2] = {n, n};
	int starts[2]   = {1, 1};

	MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &matrix);
	MPI_Type_commit(&matrix);

	int *counts = new int[size];
	int *displs = new int[size];

	int counter = 0;
	for (int i = 0; i < px; i++)
	{
		for (int j = 0; j < px; j++)
		{
			counts[counter] = 1;
			displs[counter] = i*N+j; // i*(n*N) + j отступ в int
			counter++;
		}
	}

	// if (rank == 0)
	// {
	// 	for (int i = 0; i < N; i++)
	// 	{
	// 		for (int j = 0; j < N; j++)
	// 		{
	// 			cout << global_data[i*N + j] << " ";
	// 		}
	// 		cout << endl;
	// 	}
	// 	cout << endl;
	// }

	MPI_Scatterv(global_data, counts, displs, global_matrix, data, 1, matrix, 0, MPI_COMM_WORLD);

	// for (int i = 0; i < size; i++)
	// {
	// 	if (rank == i)
	// 	{
	// 		cout << rank << endl;
	// 		for (int i = 1; i < n+1; i++)
	// 		{
	// 			for (int j = 1; j < n+1; j++)
	// 			{
	// 				cout << data[i*(n+2) + j] << " ";
	// 			}
	// 			cout << endl;
	// 		}
	// 	}
	// 	MPI_Barrier(MPI_COMM_WORLD);
	// }

	// if (rank == 0)
	// {
	// 	for (int i = 0; i < N; i++)
	// 	{
	// 		for (int j = 0; j < N; j++)
	// 		{
	// 			cout << global_temp[i*N + j] << " ";
	// 		}
	// 		cout << endl;
	// 	}
	// 	cout << endl;
	// }

	MPI_Gatherv(data, 1, matrix, global_temp, counts, displs, global_matrix, 0, MPI_COMM_WORLD);

	// if (rank == 0)
	// {
	// 	for (int i = 0; i < N; i++)
	// 	{
	// 		for (int j = 0; j < N; j++)
	// 		{
	// 			cout << global_temp[i*N + j] << " ";
	// 		}
	// 		cout << endl;
	// 	}
	// 	cout << endl;
	// }
	
	return;
	
	for( int t = 0; t < T; t++ )
	{
		setup_boundaries(n, data);
		update_data(n, data, temp);
		swap(data, temp);
	}

	ofstream f("output.dat");
	for( int i=1; i<=n; i++ )
	{
		for( int j=1; j<=n; j++ )
			f << data[i*(n+2)+j];
		f << endl;
	}
	f.close();

	delete[] counts;
	delete[] displs;

	if (rank == 0)
	{
		delete[] global_data;
		delete[] global_temp;
		MPI_Type_free(&global_matrix);
	}

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
