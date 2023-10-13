#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


double phi(double x)
{
	return sin(3*x);
}


double psi(double x)
{
	return -2*sin(3*x);
}


double f(double a, double t, double x)
{
	return (9*a*a - 4)*cos(2*t)*sin(3*x);
}

// u(t, 0)
double mu_left(double t, double x)
{
	return 0;
}

// u(t, L)
double mu_right(double t, double x)
{
	return cos(2*t)*sin(3*x);
}


double u(double t, double x)
{
	return cos(2*t)*sin(3*x);
}


int main(int argc, char** argv)
{
	const clock_t start_time = clock();

	if (argc != 3)
	{
		printf("Use: %s N M\n", argv[0]);
		return 1;
	}

	const int
		N = atoi(argv[1]),
		M = atoi(argv[2]);

	const double
		T = 1,
		L = 1,
		tau = T / N,
		h = L / M,
		a = 1;

	double
		norm_L2 = 0,
		norm_C = 0,
		*A[3];

	for (int i = 0; i < 3; ++i)
	{
		A[i] = malloc((M+1) * sizeof(double));
	}

	double t = 0;

	for (int i = 0; i <= M; ++i)
	{
		A[1][i] = phi(i*h);
	}

	for (int i = 1; i < M; ++i)
	{
		A[2][i] = A[1][i] + tau*(psi(i*h)
			+ tau/2*(a*a * (A[1][i+1] - 2*A[1][i] + A[1][i-1]) / (h*h) + f(a, t, i*h)));
	}

	t += tau;

	A[2][0] = mu_left(t, 0);
	A[2][M] = mu_right(t, L);

	while (t < T - tau/2)
	{
		double* tmp = A[0];
		A[0] = A[1];
		A[1] = A[2];
		A[2] = tmp;

		for (int i = 1; i < M; ++i)
		{
			A[2][i] = 2*A[1][i] - A[0][i]
				+ tau*tau*(a*a * (A[1][i+1] - 2*A[1][i] + A[1][i-1]) / (h*h) + f(a, t, i*h));
		}

		t += tau;

		A[2][0] = mu_left(t, 0);
		A[2][M] = mu_right(t, L);
	}

	for (int i = 0; i <= M; ++i)
	{
		const double dif = u(T, i*h) - A[2][i];
		norm_L2 += dif*dif;
		norm_C = fmax(norm_C, fabs(dif));
	}
	norm_L2 = sqrt(h*norm_L2);

	for (int i = 0; i < 3; ++i)
	{
		free(A[i]);
	}

	printf("%.16lf %.16lf %lf\n", norm_L2, norm_C, (double)(clock() - start_time) / CLOCKS_PER_SEC);

	return 0;
}
