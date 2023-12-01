#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>


#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define il(ig) ((ig) - i_min_overlap)
#define jl(jg) ((jg) - j_min_overlap)

#define LEFT_TO_RIGHT 1
#define RIGHT_TO_LEFT 2
#define TOP_TO_BOT 3
#define BOT_TO_TOP 4


double phi(double x, double y);
double psi(double x, double y);
double f(double a2, double t, double x, double y);
double mu_left(double t, double x, double y);
double mu_bot(double t, double x, double y);
double mu_right(double t, double x, double y);
double mu_top(double t, double x, double y);
double u(double t, double x, double y);
int unstable(double a, double tau, double hx2, double hy2);
void apply_bc(double Lx, double Ly, int Mx, int My, double hx, double hy, int i_min, int i_max, int j_min, int j_max, int i_min_overlap, int j_min_overlap, double t, double*** A, int ib, int ie);


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 7)
    {
        if (!rank)
            printf("Use: OMP_NUM_THREADS=n mpirun --bind-to none %s T Lx Ly N Mx My\n", argv[0]);
        return -1;
    }
    
    const int
        N = atoi(argv[4]),
        Mx = atoi(argv[5]) + 1,
        My = atoi(argv[6]) + 1;

    const double
        T = atof(argv[1]),
        Lx = atof(argv[2]),
        Ly = atof(argv[3]),
        tau = T / N,
        hx = Lx / (Mx-1),
        hy = Ly / (My-1),
        a = 0.7,
        
        hx2 = hx*hx,
        hy2 = hy*hy,
        a2 = a*a,
        tau2 = tau*tau,
        tau_2 = tau/2;

    if (unstable(a, tau, hx2, hy2))
    {
        if (!rank)
            printf("unstable\n");
        return -2;
    }

    double **A[3], elapsed_time, norm_L2, norm_C;

    MPI_Comm comm;
    int dims[] = {0, 0};
    MPI_Dims_create(size, 2, dims);
    int periods[] = {0, 0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm);
    MPI_Comm_rank(comm, &rank);
    int coords[2];
    MPI_Cart_coords(comm, rank, 2, coords);
    int left, right, top, bot;
    MPI_Cart_shift(comm, 0, 1, &left, &right);
    MPI_Cart_shift(comm, 1, 1, &top, &bot);

    const int
        i_rank = coords[1],
        i_remainder = My % dims[1],
        i_size = My / dims[1] + (i_rank < i_remainder ? 1 : 0),
        i_min = i_rank * (My / dims[1]) + (i_rank < i_remainder ? i_rank : i_remainder),
        i_max = i_min + i_size,
        i_min_overlap = i_min > 0 ? i_min - 1 : i_min,
        i_max_overlap = i_max < My ? i_max + 1 : i_max,
        i_size_overlap = i_max_overlap - i_min_overlap,
        ib = max(1, i_min),
        ie = min(My-1, i_max),

        j_rank = coords[0],
        j_remainder = Mx % dims[0],
        j_size = Mx / dims[0] + (j_rank < j_remainder ? 1 : 0),
        j_min = j_rank * (Mx / dims[0]) + (j_rank < j_remainder ? j_rank : j_remainder),
        j_max = j_min + j_size,
        j_min_overlap = j_min > 0 ? j_min - 1 : j_min,
        j_max_overlap = j_max < Mx ? j_max + 1 : j_max,
        j_size_overlap = j_max_overlap - j_min_overlap,
        jb = max(1, j_min),
        je = min(Mx-1, j_max);

    MPI_Datatype row, column;
    MPI_Type_contiguous(j_size, MPI_DOUBLE, &row);
    MPI_Type_vector(i_size, 1, j_size_overlap, MPI_DOUBLE, &column);
    MPI_Type_commit(&row);
    MPI_Type_commit(&column);

    double* tmp[3];
    for (int k = 0; k < 3; ++k)
    {
        tmp[k] = malloc(i_size_overlap * j_size_overlap * sizeof(double));
        A[k] = malloc(i_size_overlap * sizeof(double*));
    }

    #pragma omp parallel
    {
        for (int k = 0; k < 3; ++k)
        {
            #pragma omp for nowait
            for (int i = 0; i < i_size_overlap; ++i)
            {
                A[k][i] = &tmp[k][i * j_size_overlap];
            }
        }

        double t = 0;

        #pragma omp for
        for (int ig = i_min_overlap; ig < i_max_overlap; ++ig)
        {
            double y = ig*hy;
            int i = il(ig);

            for (int jg = j_min_overlap; jg < j_max_overlap; ++jg)
            {
                double x = jg*hx;
                int j = jl(jg);

                A[1][i][j] = phi(x, y);
            }
        }

        double tn = t;
        t += tau;

        #pragma omp for nowait
        for (int ig = ib; ig < ie; ++ig)
        {
            double y = ig*hy;
            int i = il(ig);

            for (int jg = jb; jg < je; ++jg)
            {
                double x = jg*hx;
                int j = jl(jg);

                A[2][i][j] = A[1][i][j] + tau*(psi(x, y)
                    + tau_2*(a2 * ((A[1][i+1][j] - 2*A[1][i][j] + A[1][i-1][j])/hy2
                    + (A[1][i][j+1] - 2*A[1][i][j] + A[1][i][j-1])/hx2) + f(a2, tn, x, y)));
            }
        }

        apply_bc(Lx, Ly, Mx, My, hx, hy, i_min, i_max, j_min, j_max, i_min_overlap, j_min_overlap, t, A, ib, ie);

        #pragma omp single
        MPI_Barrier(comm);
        #pragma omp master
        if (!rank)
        {
            elapsed_time = MPI_Wtime();
        }

        while (t < T - tau_2)
        {
            tn = t;
            t += tau;

            #pragma omp single
            {
                for (int i = 0; i < 2; i++)
                {
                    if (j_rank % 2 == i)
                    {
                        MPI_Send(&A[2][il(i_min)][jl(j_max-1)], 1, column, right, LEFT_TO_RIGHT, comm);
                        MPI_Send(&A[2][il(i_min)][jl(j_min)], 1, column, left, RIGHT_TO_LEFT, comm);
                    }
                    else
                    {
                        MPI_Recv(&A[2][il(i_min)][jl(j_min_overlap)], 1, column, left, LEFT_TO_RIGHT, comm, MPI_STATUS_IGNORE);
                        MPI_Recv(&A[2][il(i_min)][jl(j_max_overlap-1)], 1, column, right, RIGHT_TO_LEFT, comm, MPI_STATUS_IGNORE);
                    }

                    if (i_rank % 2 == i)
                    {
                        MPI_Send(&A[2][il(i_max-1)][jl(j_min)], 1, row, bot, TOP_TO_BOT, comm);
                        MPI_Send(&A[2][il(i_min)][jl(j_min)], 1, row, top, BOT_TO_TOP, comm);
                    }
                    else
                    {
                        MPI_Recv(&A[2][il(i_min_overlap)][jl(j_min)], 1, row, top, TOP_TO_BOT, comm, MPI_STATUS_IGNORE);
                        MPI_Recv(&A[2][il(i_max_overlap-1)][jl(j_min)], 1, row, bot, BOT_TO_TOP, comm, MPI_STATUS_IGNORE);
                    }
                }

                double** tmp = A[0];
                A[0] = A[1];
                A[1] = A[2];
                A[2] = tmp;
            }

            #pragma omp for nowait
            for (int ig = ib; ig < ie; ++ig)
            {
                double y = ig*hy;
                int i = il(ig);

                for (int jg = jb; jg < je; ++jg)
                {
                    double x = jg*hx;
                    int j = jl(jg);

                    A[2][i][j] = 2*A[1][i][j] - A[0][i][j]
                        + tau2*(a2*((A[1][i+1][j] - 2*A[1][i][j] + A[1][i-1][j])/hy2
                        + (A[1][i][j+1] - 2*A[1][i][j] + A[1][i][j-1])/hx2) + f(a2, tn, x, y));
                }
            }

            apply_bc(Lx, Ly, Mx, My, hx, hy, i_min, i_max, j_min, j_max, i_min_overlap, j_min_overlap, t, A, ib, ie);

            #pragma omp barrier
        }

        #pragma omp single
        MPI_Barrier(comm);
        #pragma omp master
        if (!rank)
        {
            elapsed_time = MPI_Wtime() - elapsed_time;
        }

        #pragma omp for reduction(+ : norm_L2) reduction(max : norm_C)
        for (int ig = i_min; ig < i_max; ++ig)
        {
            double y = ig*hy;
            int i = il(ig);

            for (int jg = j_min; jg < j_max; ++jg)
            {
                double x = jg*hx;
                int j = jl(jg);

                double dif = u(T, x, y) - A[2][i][j];
                norm_L2 += dif*dif;
                norm_C = fmax(norm_C, fabs(dif));
            }
        }

        #pragma omp master
        {
            if (rank)
            {
                MPI_Reduce(&norm_L2, NULL, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
                MPI_Reduce(&norm_C, NULL, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
            }
            else
            {
                MPI_Reduce(MPI_IN_PLACE, &norm_L2, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
                MPI_Reduce(MPI_IN_PLACE, &norm_C, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
                norm_L2 = sqrt(hx*hy*norm_L2);
                printf("%.16lf %.16lf %.16lf %lf %lf %lf %d %d %d %d %d %d %d\n", norm_L2, norm_C, elapsed_time, T, Lx, Ly, N, Mx-1, My-1, omp_get_num_threads(), size, dims[0], dims[1]);
            }
        }
    }

    for (int k = 0; k < 3; ++k)
    {
        free(tmp[k]);
        free(A[k]);
    }

    MPI_Type_free(&row);
    MPI_Type_free(&column);

    MPI_Finalize();

    return 0;
}


double phi(double x, double y)
{
    return sin(4*x)*cos(5*y);
}


double psi(double x, double y)
{
    return 6*cos(7*x)*sin(8*y);
}


double f(double a2, double t, double x, double y)
{
    return -9*cos(3*t)*sin(4*x)*cos(5*y) - 36*sin(6*t)*cos(7*x)*sin(8*y) + a2*(41*cos(3*t)*sin(4*x)*cos(5*y) + 113*sin(6*t)*cos(7*x)*sin(8*y));
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


int unstable(double a, double tau, double hx2, double hy2)
{
    return a*tau*sqrt(1/hx2 + 1/hy2) > 1;
}


void apply_bc(double Lx, double Ly, int Mx, int My, double hx, double hy, int i_min, int i_max, int j_min, int j_max, int i_min_overlap, int j_min_overlap, double t, double*** A, int ib, int ie)
{
    if (i_min == 0)
    {
        int i = il(i_min);

        #pragma omp for nowait
        for (int jg = j_min; jg < j_max; ++jg)
        {
            double x = jg*hx;
            int j = jl(jg);
            A[2][i][j] = mu_bot(t, x, 0);
        }
    }

    if (i_max == My)
    {
        int i = il(i_max-1);

        #pragma omp for nowait
        for (int jg = j_min; jg < j_max; ++jg)
        {
            double x = jg*hx;
            int j = jl(jg);
            A[2][i][j] = mu_top(t, x, Ly);
        }
    }

    if (j_min == 0)
    {
        int j = jl(j_min);

        #pragma omp for nowait
        for (int ig = ib; ig < ie; ++ig)
        {
            double y = ig*hy;
            int i = il(ig);
            A[2][i][j] = mu_left(t, 0, y);
        }
    }

    if (j_max == Mx)
    {
        int j = jl(j_max-1);

        #pragma omp for nowait
        for (int ig = ib; ig < ie; ++ig)
        {
            double y = ig*hy;
            int i = il(ig);
            A[2][i][j] = mu_right(t, Lx, y);
        }
    }
}
