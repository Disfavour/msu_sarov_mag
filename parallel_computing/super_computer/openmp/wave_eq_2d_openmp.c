#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


double phi(double x, double y)
{
	return sin(4*x)*cos(5*y);
}


double psi(double x, double y)
{
	return 6*cos(7*x)*sin(8*y);
}


double f(double a, double t, double x, double y)
{
	return -9*cos(3*t)*sin(4*x)*cos(5*y) - 36*sin(6*t)*cos(7*x)*sin(8*y) + a*a*(41*cos(3*t)*sin(4*x)*cos(5*y) + 113*sin(6*t)*cos(7*x)*sin(8*y));
}

// u(t, 0, y)
double mu_left(double t, double x, double y)
{
	return sin(6*t)*sin(8*y);
}

// u(t, x, 0)
double mu_bot(double t, double x, double y)
{
	return cos(3*t)*sin(4*x);
}

// u(t, Lx, y)
double mu_right(double t, double x, double y)
{
	return cos(3*t)*sin(4*x)*cos(5*y) + sin(6*t)*cos(7*x)*sin(8*y);
}

// u(t, x, Ly)
double mu_top(double t, double x, double y)
{
	return cos(3*t)*sin(4*x)*cos(5*y) + sin(6*t)*cos(7*x)*sin(8*y);
}


double u(double t, double x, double y)
{
	return cos(3*t)*sin(4*x)*cos(5*y) + sin(6*t)*cos(7*x)*sin(8*y);
}


int unstable(double a, double tau, double hx, double hy)
{
	return a*tau*sqrt(1/(hx*hx) + 1/(hy*hy)) > 1;
}


int main(int argc, char** argv)
{
	const double start_time = omp_get_wtime();

	if (argc != 7)
	{
		printf("Use: %s T Lx Ly N Mx My\n", argv[0]);
		return 1;
	}

	const double
		T = atof(argv[1]),
		Lx = atof(argv[2]),
		Ly = atof(argv[3]);

	const int
		N = atoi(argv[4]),
		Mx = atoi(argv[5]),
		My = atoi(argv[6]);

	const double
		tau = T / N,
		hx = Lx / Mx,
		hy = Ly / My,
		a = 1./16;

	if (unstable(a, tau, hx, hy))
	{
		printf("unstable\n");
		return 2;
	}

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
		
		#pragma omp single nowait
		printf("%.16lf %.16lf %.16lf %lf %lf %lf %d %d %d %d\n", norm_L2, norm_C, omp_get_wtime() - start_time, T, Lx, Ly, N, Mx, My, omp_get_num_threads());
	}

	return 0;
}
