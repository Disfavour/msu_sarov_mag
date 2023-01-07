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
		max_d = 3 * ((N - 2) - 1),
		k_len = (N - 2) / STEP;

	#pragma omp parallel reduction(max : eps)
	{
		for (int d = 0; d <= max_d; ++d)
		{
			const int
				tmp_k = d - 2 * ((N - 2) - 1),
				min_k = tmp_k > 0 ? tmp_k : 0,
				max_k = d < k_len ? d : k_len - 1;

			#pragma omp for
			for (int column = min_k; column <= max_k; ++column)
			{
				const int
					tmp_j = d - ((N - 2) - 1) - column,
					min_j = tmp_j > 0 ? tmp_j : 0,
					max_j = d - column < (N - 2) ? d - column : ((N - 2) - 1);

				for (int row = min_j; row <= max_j; ++row)
				{
					const int
						i = d - row - column + 1,
						j = row + 1,
						start_k = column * STEP + 1,
						end_k = start_k + STEP;

					#pragma omp simd
					for (int k = start_k; k < end_k; ++k)
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
