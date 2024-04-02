#include <iostream>
#include <cmath>
#include <mpi.h>
#include <fstream>
using namespace std;

int func = 0;
int migrate_size = -1;

double frand() // вещественное случайное число в диапазоне [0,1)
{
	return double(rand())/RAND_MAX;
}

double Sphere(int *a, int n)
{
    double sum = 0;
    for(int i = 0 ; i < n; ++i)
	{
        sum += pow(a[i], 2);
    }
    return sum;
}

double Rozenbrok(int *a, int n)
{
    double sum = 0;
    for(int i = 0 ; i < n - 1; ++i)
	{
        sum += 100 * pow(pow(a[i], 2) - a[i + 1], 2) + pow(a[i] - 1, 2);
    }
    return sum;
}

double Rastrigin(int *a, int n)
{
    double sum = 0;
    for(int i = 0 ; i < n; ++i) {
        sum += pow(a[i], 2) - 10 * cos(2 * M_PI * a[i]) + 10;
    }
    return sum;
}

double eval(int* a, int n)
{
	double sum = 0;
	switch (func)
	{
		case 1:
			sum = Sphere(a, n);
			break;
		case 2:
			sum = Rozenbrok(a, n);
			break;
		case 3:
			sum = Rastrigin(a, n);
			break;
		default:
			cout << func << endl;
			exit(123);
	}

	return -sum;
}

int rand_coord()
{
	return rand() % 201 - 100;
}

void init(int* P, int m, int n)
{
	for( int k=0; k<m; k++ )
		for( int i=0; i<n; i++ )
			P[k*n+i] = rand_coord();
}

void shuffle(int* P, int m, int n)
{
	for( int k=0; k<m; k++ )
	{
		int l = rand()%m;
		for( int i=0; i<n; i++ )
			swap(P[k*n+i],P[l*n+i]);
	}
}

void select(int* P, int m, int n)
{
	double
		p_tournier = 0.7,
		p_win = 0.1;

	shuffle(P, m, n);
	for( int k=0; k<m/2; k++ )
	{
		int a = 2*k;
		int b = 2*k+1;
		int fa = eval(P+a*n, n);
		int fb = eval(P+b*n, n);

		if (frand() < p_tournier)
		{
			if (fa >= fb && !(frand() < p_win) || fa < fb && frand() < p_win)
			{
				for (int i=0; i<n; i++)
					P[b*n+i] = P[a*n+i];
			}
			else
			{
				for (int i=0; i<n; i++)
					P[a*n+i] = P[b*n+i];
			}
		}
	}
}

void crossover(int* P, int m, int n)
{
	shuffle(P, m, n);
	for( int k=0; k<m/2; k++ )
	{
		int a = 2*k;
		int b = 2*k+1;
		int j = rand()%n;
		for( int i=j; i<n; i++ )
			swap(P[a*n+i],P[b*n+i]);
	}
}

void mutate(int* P, int m, int n)
{
	double pmut = 0.01;
	for( int k=0; k<m; k++ )
		for( int i=0; i<n; i++ )
			if( frand()<pmut )
				P[k*n+i] = rand_coord();
}

void get_avg_and_best(int* P, int m, int n, double& f_avg, double& f_best)
{
	f_best = eval(P, n);
	double f_sum = eval(P, n);

	for( int k=1; k<m; k++)
	{
		double f = eval(P+k*n, n);
		f_sum += f;

		if(f > f_best)
		{
			f_best = f;
		}
	}

	f_avg = -f_sum / m;
	f_best = -f_best;
}

void migrate(int* P, int m, int n)
{
	shuffle(P, m, n);

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int
		left = rank - 1 >= 0 ? rank - 1 : size - 1,
		right = rank + 1 < size ? rank + 1 : 0;
	
	MPI_Sendrecv_replace(P, migrate_size, MPI_INT, right, 123, left, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void runGA(int n, int m, int T)
{
	double* f_avg = new double[T];
	double* f_best = new double[T];

	int* P = new int[n*m];
	init(P, m, n);
	for(int t = 0; t<T; t++)
	{
		if (t > 0 && t % 50 == 0)
			migrate(P, m, n);

		select(P, m, n);
		crossover(P, m, n);
		mutate(P, m, n);
		get_avg_and_best(P, m, n, f_avg[t], f_best[t]);
	}

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank > 0)
	{
		MPI_Reduce(f_avg, NULL, T, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

		MPI_Reduce(f_best, NULL, T, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Reduce(MPI_IN_PLACE, f_avg, T, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

		MPI_Reduce(MPI_IN_PLACE, f_best, T, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

		for (int i = 0; i < T; i++)
		{
			f_avg[i] /= size;
		}

		ofstream f("output.dat");
		for(int t = 0; t<T; t++)
			f << f_avg[t] << " ";
		f << endl;
		for(int t = 0; t<T; t++)
			f << f_best[t] << " ";
		f.close();
		
		cout << f_best[T-1] << endl;
	}

	delete[] P;
	delete[] f_avg;
	delete[] f_best;
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	srand(time(NULL) + rank);

	int n = 13; //atoi(argv[1]);
	int m = 80000; // atoi(argv[2]);
	int T = 1000; // atoi(argv[3]);

	func = 3;
	migrate_size = m / (size * 2);

	const int m_local = m / size + (m % size > 0 ? rank < m % size : 0);

	runGA(n, m_local, T);

	MPI_Finalize();

	return 0;
}
