#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


double phi(double x, double y)
{
	return sin(3*x)*cos(4*y);
}


double psi(double x, double y)
{
	return 0;
}


double f(double a, double t, double x, double y)
{
	return (25*a*a - 4)*cos(2*t)*sin(3*x)*cos(4*y);
}

// u(t, 0, y)
double mu_left(double t, double x, double y)
{
	return 0;
}

// u(t, x, 0)
double mu_bot(double t, double x, double y)
{
	return cos(2*t)*sin(3*x);
}

// u(t, Lx, y)
double mu_right(double t, double x, double y)
{
	return cos(2*t)*sin(3*x)*cos(4*y);
}

// u(t, x, Ly)
double mu_top(double t, double x, double y)
{
	return cos(2*t)*sin(3*x)*cos(4*y);
}


double u(double t, double x, double y)
{
	return cos(2*t)*sin(3*x)*cos(4*y);
}


int main(int argc, char** argv)
{
	const double start_time = omp_get_wtime();

	if (argc != 4)
	{
		printf("Use: %s N Mx My\n", argv[0]);
		return 1;
	}

	const int
		N = atoi(argv[1]),
		Mx = atoi(argv[2]),
		My = atoi(argv[3]);

	const double
		T = 1,
		Lx = 1,
		Ly = 1,
		tau = T / N,
		hx = Lx / Mx,
		hy = Ly / My,
		a = 1;

	double
		norm_L2 = 0,
		norm_C = 0,
		**A[3];

	#pragma omp parallel
	{
		for (int i = 0; i < 3; ++i)
		{
			#pragma omp single
			A[i] = malloc((My+1) * sizeof(double*));

			#pragma omp for nowait
			for (int j = 0; j <= My; ++j)
			{
				A[i][j] = malloc((Mx+1) * sizeof(double));
			}
		}

		double t = 0;

		#pragma omp for
		for (int i = 0; i <= My; ++i)
		{
			for (int j = 0; j <= Mx; ++j)
			{
				A[1][i][j] = phi(j*hx, i*hy);
			}
		}

		#pragma omp for nowait
		for (int i = 1; i < My; ++i)
		{
			for (int j = 1; j < Mx; ++j)
			{
				A[2][i][j] = A[1][i][j] + tau*(psi(j*hx, i*hy)
					+ tau/2*(a*a * ((A[1][i+1][j] - 2*A[1][i][j] + A[1][i-1][j])/(hy*hy)
					+ (A[1][i][j+1] - 2*A[1][i][j] + A[1][i][j-1])/(hx*hx)) + f(a, t, j*hx, i*hy)));
			}
		}

		t += tau;

		#pragma omp for nowait
		for (int i = 1; i < My; ++i)
		{
			A[2][i][0] = mu_left(t, 0, i*hy);
			A[2][i][Mx] = mu_right(t, Lx, i*hy);
		}

		#pragma omp for
		for (int j = 0; j <= Mx; ++j)
		{
			A[2][My][j] = mu_top(t, j*hx, Ly);
			A[2][0][j] = mu_bot(t, j*hx, 0);
		}

		while (t < T - tau/2)
		{
			#pragma omp single
			{
				double** tmp = A[0];
				A[0] = A[1];
				A[1] = A[2];
				A[2] = tmp;
			}

			#pragma omp for nowait
			for (int i = 1; i < My; ++i)
			{
				for (int j = 1; j < Mx; ++j)
				{
					A[2][i][j] = 2*A[1][i][j] - A[0][i][j]
						+ tau*tau*(a*a*((A[1][i+1][j] - 2*A[1][i][j] + A[1][i-1][j])/(hy*hy)
						+ (A[1][i][j+1] - 2*A[1][i][j] + A[1][i][j-1])/(hx*hx)) + f(a, t, j*hx, i*hy));
				}
			}

			t += tau;

			#pragma omp for nowait
			for (int i = 1; i < My; ++i)
			{
				A[2][i][0] = mu_left(t, 0, i*hy);
				A[2][i][Mx] = mu_right(t, Lx, i*hy);
			}

			#pragma omp for
			for (int j = 0; j <= Mx; ++j)
			{
				A[2][My][j] = mu_top(t, j*hx, Ly);
				A[2][0][j] = mu_bot(t, j*hx, 0);
			}
		}

		#pragma omp for reduction(+ : norm_L2) reduction(max : norm_C)
		for (int i = 0; i <= My; ++i)
		{
			for (int j = 0; j <= Mx; ++j)
			{
				const double dif = u(T, j*hx, i*hy) - A[2][i][j];
				norm_L2 += dif*dif;
				norm_C = fmax(norm_C, fabs(dif));
			}
		}

		#pragma omp single nowait
		norm_L2 = sqrt(hx*hy*norm_L2);

		for (int i = 0; i < 3; ++i)
		{
			#pragma omp for
			for (int j = 0; j <= My; ++j)
			{
				free(A[i][j]);
			}

			#pragma omp single nowait
			free(A[i]);
		}
	}

	printf("%.16lf %.16lf %.16lf\n", norm_L2, norm_C, omp_get_wtime() - start_time);

	return 0;	// OMP_NUM_THREADS=16 ./a.out 1600 800 800 0.0000005066379303 0.0000010912610420 3.8504993290007405
}
