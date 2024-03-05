typedef struct
{
    int n, m;
    double **data;
} matrix;

void print_matrix(matrix A);
void init_matrix(matrix A);
matrix create_matrix(int n, int m);
void delete_matrix(matrix* A);
matrix add(matrix A, matrix B);
matrix multiply(matrix A, matrix B);
