#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <mpi.h>


using namespace std;

double frand(double a, double b)
{
	return a+(b-a)*(rand()/double(RAND_MAX));
}

int do_walk(int a, int b, int x, double p, double& t)
{
	int step = 0;
	while( x>a && x<b )
	{
		if( frand(0,1)<p )
			x += 1;
		else
			x -= 1;
		t += 1.0;
		step += 1;
	}
	return x;
}

void run_mc(int a, int b, int x, double p, int N, int& w, double& t)
{
	for( int i=0; i<N; i++ )
	{
		int out = do_walk(a, b, x, p, t);
		if( out == b )
			w += 1;
	}
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	srand(time(NULL) + rank);

	int a = atoi(argv[1]);
	int b = atoi(argv[2]);
	int x = atoi(argv[3]);
	double p = atof(argv[4]);
	int N = atoi(argv[5]);

	const int N_local = N / size + (N % size > 0 ? rank < N % size : 0);

	int w = 0;
	double t = 0;
	double elapsed_time = 0;

	if (!rank)
	{
		elapsed_time = MPI_Wtime();
	}

	run_mc(a, b, x, p, N_local, w, t);

	if (rank > 0)
	{
		MPI_Reduce(&w, NULL, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

		MPI_Reduce(&t, NULL, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Reduce(MPI_IN_PLACE, &w, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

		MPI_Reduce(MPI_IN_PLACE, &t, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

		elapsed_time = MPI_Wtime() - elapsed_time;

		ofstream f1("output.dat");
		f1 << double(w)/N << " " << t/N << endl;
		f1.close();

		ofstream f2("stat.dat");
		f2 << a << " " << b << " " << x << " " << p << " " << N << " " << size << " " << elapsed_time << endl;
		f2.close();

		ofstream f3("data_for_plots.dat", ios::app);
		f3 << N << " " << size << " " << elapsed_time << endl;
		f3.close();
	}

	MPI_Finalize();

	return 0;
}
