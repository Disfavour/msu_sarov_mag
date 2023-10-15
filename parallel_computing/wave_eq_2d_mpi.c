#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


#define maxi(a,b) ((a)>(b)?(a):(b))
#define mini(a,b) ((a)<(b)?(a):(b))

#define TAG_TOP_TO_BOT 123
#define TAG_BOT_TO_TOP 321


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
	MPI_Init(&argc, &argv);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double start_time;

	if (!rank)
	{
		start_time = MPI_Wtime();
	}

	if (argc != 4)
	{
		if (!rank)
		{
			printf("Use: %s N Mx My\n", argv[0]);
		}

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
		norm_L2_global,
		norm_C_global,
		**A[3];

	const int
		i_size = (My+1) / size + (rank < (My+1) % size ? 1 : 0),
		i_size_with_overlap = i_size + (!rank || rank == size-1 ? 1 : 2),
		i_min = rank < (My+1) % size ? i_size * rank : (My+1) - (size-rank)*i_size,
		i_max = i_min + i_size,
		i_min_with_overlap = i_min - (rank > 0),
		i_max_with_overlap = i_max + (rank < size-1);

	for (int k = 0; k < 3; ++k)
	{
		A[k] = malloc(i_size_with_overlap * sizeof(double*));

		for (int i = 0; i < i_size_with_overlap; ++i)
		{
			A[k][i] = malloc((Mx+1) * sizeof(double));
		}
	}

	double t = 0;

	for (int i_ = i_min_with_overlap; i_ < i_max_with_overlap; ++i_)
	{
		for (int j = 0; j <= Mx; ++j)
		{
			const int i = i_ - i_min_with_overlap;
			A[1][i][j] = phi(j*hx, i_*hy);
		}
	}

	for (int i_ = maxi(1, i_min); i_ < mini(My, i_max); ++i_)
	{
		for (int j = 1; j < Mx; ++j)
		{
			const int i = i_ - i_min_with_overlap;
			A[2][i][j] = A[1][i][j] + tau*(psi(j*hx, i_*hy)
				+ tau/2*(a*a * ((A[1][i+1][j] - 2*A[1][i][j] + A[1][i-1][j])/(hy*hy)
				+ (A[1][i][j+1] - 2*A[1][i][j] + A[1][i][j-1])/(hx*hx)) + f(a, t, j*hx, i_*hy)));
		}
	}

	t += tau;

	for (int i_ = maxi(1, i_min); i_ < mini(My, i_max); ++i_)
	{
		const int i = i_ - i_min_with_overlap;
		A[2][i][0] = mu_left(t, 0, i_*hy);
		A[2][i][Mx] = mu_right(t, Lx, i_*hy);
	}

	if (!rank)
	{
		for (int j = 0; j <= Mx; ++j)
		{
			A[2][i_min - i_min_with_overlap][j] = mu_bot(t, j*hx, 0); // 0
		}
	}
	else if (rank == size - 1)
	{
		for (int j = 0; j <= Mx; ++j)
		{
			A[2][i_max-1 - i_min_with_overlap][j] = mu_top(t, j*hx, Ly); // My
		}
	}

	const int
			bot = (rank - 1 < 0) ? (MPI_PROC_NULL) : (rank - 1),
			top = (rank + 1 >= size) ? (MPI_PROC_NULL) : (rank + 1);

	while (t < T - tau/2)
	{
		MPI_Request request[4];

		MPI_Irecv(A[2][i_max - i_min_with_overlap], Mx+1, MPI_DOUBLE, top, TAG_TOP_TO_BOT, MPI_COMM_WORLD, &request[0]);

		MPI_Irecv(A[2][i_min_with_overlap - i_min_with_overlap], Mx+1, MPI_DOUBLE, bot, TAG_BOT_TO_TOP, MPI_COMM_WORLD, &request[1]);

		MPI_Isend(A[2][i_max-1 - i_min_with_overlap], Mx+1, MPI_DOUBLE, top, TAG_BOT_TO_TOP, MPI_COMM_WORLD, &request[2]);

		MPI_Isend(A[2][i_min - i_min_with_overlap], Mx+1, MPI_DOUBLE, bot, TAG_TOP_TO_BOT, MPI_COMM_WORLD, &request[3]);

		MPI_Waitall(4, request, MPI_STATUS_IGNORE);

		double** tmp = A[0];
		A[0] = A[1];
		A[1] = A[2];
		A[2] = tmp;

		for (int i_ = maxi(1, i_min); i_ < mini(My, i_max); ++i_)
		{
			for (int j = 1; j < Mx; ++j)
			{
				const int i = i_ - i_min_with_overlap;
				A[2][i][j] = 2*A[1][i][j] - A[0][i][j]
					+ tau*tau*(a*a*((A[1][i+1][j] - 2*A[1][i][j] + A[1][i-1][j])/(hy*hy)
					+ (A[1][i][j+1] - 2*A[1][i][j] + A[1][i][j-1])/(hx*hx)) + f(a, t, j*hx, i_*hy));
			}
		}

		t += tau;

		for (int i_ = maxi(1, i_min); i_ < mini(My, i_max); ++i_)
		{
			const int i = i_ - i_min_with_overlap;
			A[2][i][0] = mu_left(t, 0, i_*hy);
			A[2][i][Mx] = mu_right(t, Lx, i_*hy);
		}

		if (!rank)
		{
			for (int j = 0; j <= Mx; ++j)
			{
				A[2][i_min - i_min_with_overlap][j] = mu_bot(t, j*hx, 0); // 0
			}
		}
		else if (rank == size - 1)
		{
			for (int j = 0; j <= Mx; ++j)
			{
				A[2][i_max-1 - i_min_with_overlap][j] = mu_top(t, j*hx, Ly); // My
			}
		}
	}

	for (int i_ = i_min; i_ < i_max; ++i_)
	{
		for (int j = 0; j <= Mx; ++j)
		{
			const int i = i_ - i_min_with_overlap;
			const double dif = u(T, j*hx, i_*hy) - A[2][i][j];
			norm_L2 += dif*dif;
			norm_C = fmax(norm_C, fabs(dif));
		}
	}

	MPI_Reduce(&norm_L2, &norm_L2_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (!rank)
	{
		norm_L2_global = sqrt(hx*hy*norm_L2_global);
	}

	MPI_Reduce(&norm_C, &norm_C_global, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	for (int k = 0; k < 3; ++k)
	{
		for (int i = 0; i < i_size_with_overlap; ++i)
		{
			free(A[k][i]);
		}
		free(A[k]);
	}

	if (!rank)
	{
		printf("%.16lf %.16lf %.16lf\n", norm_L2_global, norm_C_global, MPI_Wtime() - start_time);
	}

	MPI_Finalize();

	return 0;
}
