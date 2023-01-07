#ifndef __matrix_h__
#define __matrix_h__

int** get_matrix(int n, int m);

void free_matrix(int** A, int n);

void scanf_matrix(int** A, int n, int m);

void printf_matrix(int** A, int n, int m);

void make_rand_matrix(int** A, int n, int m, int max);

int get_zero_col_count(int** A, int n, int m);

void saddle_points(int** A, int n, int m);

void fill_matrix_from_file(int** A, int n, int m, char* fname);

void write_matrix_to_file(int** A, int n, int m, char* fname);

void fill_matrix_from_binary_file(int** A, int n, int m, char* fname);

void write_matrix_to_binary_file(int** A, int n, int m, char* fname);

#endif
