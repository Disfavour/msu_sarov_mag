#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define  Max(a,b) ((a)>(b)?(a):(b))

#define  N 514
double   maxeps = 0.1e-7;
int itmax = 100;
int i,j,k;
double eps;

double A [N][N][N];

void relax();
void init();
void verify();
double dvtime_();

int main(int an, char **as)
{
        int it;

        init();

        double start_time = dvtime_();

        for(it=1; it<=itmax; it++)
        {
                eps = 0.;
                #pragma dvm actual(eps)

                relax();

                #pragma dvm get_actual(eps)
                printf( "it=%4i   eps=%f\n", it,eps);

                if (eps < maxeps) break;
        }

        printf("Elapsed time = %3lf\n", dvtime_() - start_time);

        #pragma dvm get_actual(A)

        verify();

        return 0;
}


void init()
{
        for(i=0; i<=N-1; i++)
        for(j=0; j<=N-1; j++)
        for(k=0; k<=N-1; k++)
        {
                if(i==0 || i==N-1 || j==0 || j==N-1 || k==0 || k==N-1)
                A[i][j][k]= 0.;
                else A[i][j][k]= ( 4. + i + j + k) ;
        }
}


void relax()
{
        #pragma dvm region
        {
                #pragma dvm parallel ([i][j][k]) tie(A[i][j][k]) across(A[1:1][1:1][1:1]) reduction(max(eps))
                for(i=1; i<=N-2; i++)
                for(j=1; j<=N-2; j++)
                for(k=1; k<=N-2; k++)
                {
                        double e = A[i][j][k];

                        A[i][j][k]=(A[i-1][j][k]+A[i+1][j][k]+A[i][j-1][k]+A[i][j+1][k]+A[i][j][k-1]+A[i][j][k+1])/6.;

                        eps=Max(eps, fabs(e-A[i][j][k]));
                }
        }
}


void verify()
{
        double s = 0;

        for(i=0; i<=N-1; i++)
        for(j=0; j<=N-1; j++)
        for(k=0; k<=N-1; k++)
        {
                s += A[i][j][k]*(i+1)*(j+1)*(k+1);
        }

        s /= (N*N*N);

        printf("  S = %f\n",s);
}
