#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#define  Max(a,b) ((a)>(b)?(a):(b))

#define  N 130
// Работает, если (N - 2) % STEP = 0
// Для максимального ускорения STEP = (N - 2) / omp_get_num_threads()
#define STEP 16

double maxeps = 0.1e-7;
int itmax = 100;
int i,j,k;
double eps;

double A [N][N][N];

void relax();
void init();
void verify();


int main(int an, char **as)
{
	double start_time = omp_get_wtime();

	int it;

	init();

	for(it=1; it<=itmax; it++)
	{
		eps = 0.;
		relax();
		printf( "it=%4i   eps=%f\n", it,eps);
		if (eps < maxeps)
			break;
	}

	verify();

	printf("Elapsed time = %.3lf\n", omp_get_wtime() - start_time);

	return 0;
}


void init()
{
	#pragma omp parallel for private(i, j, k)
	for(i=0; i<=N-1; i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		if(i==0 || i==N-1 || j==0 || j==N-1 || k==0 || k==N-1)
			A[i][j][k]= 0.;
		else
			A[i][j][k] = (4 + i + j + k);
	}
} 


void relax()
{
	const int
		columns = (N - 2) / STEP,
		max_d = (N - 2) + columns - 1;

	#pragma omp parallel reduction(max : eps)
	{
		for (int i = 1; i < N - 1; i++)
		{
			for (int d = 0; d < max_d; ++d)
			{
				const int
					min_column = d < (N - 2) ? 0 : d - (N - 2) + 1,
					max_column = d < columns ? d : columns - 1;

				#pragma omp for
				for (int column = min_column; column <= max_column; ++column)
				{
					const int
						j = d - column + 1,
						min_k = column * STEP + 1,
						max_k = min_k + STEP;

					#pragma omp simd
					for (int k = min_k; k < max_k; ++k)
					{
						double e = A[i][j][k];
						
						A[i][j][k]=(A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]+A[i][j][k-1]+A[i][j][k+1])/6.;
						
						eps = Max(eps, fabs(e-A[i][j][k]));
					}
				}
			}
		}
	}
}


void verify()
{ 
	double s = 0;

	#pragma omp parallel for private(i, j, k) reduction(+ : s)
	for(i=0; i<=N-1; i++)
	for(j=0; j<=N-1; j++)
	for(k=0; k<=N-1; k++)
	{
		s += A[i][j][k]*(i+1)*(j+1)*(k+1);
	}

	s /= (N*N*N);

	printf("  S = %f\n",s);
}
