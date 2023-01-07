#include <stdio.h>
#define N 5
#define M 8


void input_int(int* arr, int n);
void print_int(int* arr, int n);
void input_double(double* arr, int n);
void print_double(double* arr, int n);
int fibonacci(int n);
void max_two(int* arr, int n);
int find_index_min(double* arr, int n);
int find_index_max(double* arr, int n);
void swap_min_max(double* arr, int n);
void swap(int* first, int* second);
void bubble_sort(int arr[], int n);
int binary_search(int arr[], int left, int right, int num);


int main()
{
	// 1
	int n;

	printf("Number for fibonacci = ");
	scanf("%d", &n);

	printf("%d fibonacci number = %d\n", n, fibonacci(n));


	// 2
	printf("\nFinding two max elements (5 int in array)\n");

	int arr_int[N];

	input_int(arr_int, N);

	print_int(arr_int, N);

	max_two(arr_int, N);


	// 3
	printf("\nSwap min and max (5 double in array)\n");

	double arr_lf[N];
	input_double(arr_lf, N);
	print_double(arr_lf, N);

	printf("Swap min and max\n");
	swap_min_max(arr_lf, N);

	print_double(arr_lf, N);


	// 4
	printf("\nBinary search (8 double in array)\n");

	int arr[M];

	input_int(arr, M);

	print_int(arr, M);

	printf("Sorting array\n");

	bubble_sort(arr, M);

	print_int(arr, M);

	int num;
	printf("Index of element to find ");
	scanf("%d", &num);

	int index = binary_search(arr, 0, M - 1, num);

	printf("Index of %d in array is %d\n", num, index);

	return 0;
}


void input_int(int* arr, int n)
{
	printf("Input array: ");

	for (int i = 0; i < n; i++)
		scanf("%d", &arr[i]);
}


void print_int(int* arr, int n)
{
	printf("Array:");

	for (int i = 0; i < n; i++)
		printf(" %d", arr[i]);

	putchar('\n');
}


void input_double(double* arr, int n)
{
	printf("Input array: ");

	for (int i = 0; i < n; i++)
		scanf("%lf", &arr[i]);
}


void print_double(double* arr, int n)
{
	printf("Array:");

	for (int i = 0; i < n; i++)
		printf(" %lf", arr[i]);

	putchar('\n');
}


int fibonacci(int n)
{
	return n <= 1 ? n : fibonacci(n - 1) + fibonacci(n - 2);
}


void max_two(int* arr, int n)
{
	int
		max_first	= arr[1] > arr[0] ? arr[1] : arr[0],
		max_second	= arr[1] > arr[0] ? arr[0] : arr[1];

	for (int i = 0; i < n; i++)
	{
		if (arr[i] > max_first)
		{
			max_second = max_first;
			max_first = arr[i];
		}
		else if (arr[i] > max_second)
			max_second = arr[i];
	}

	printf("Two max numbers from array: %d, %d\n", max_first, max_second);
}


int find_index_min(double* arr, int n)
{
	int index = 0;
	double min = arr[index];

	for (int i = 0; i < n; i++)
		if (arr[i] < min)
		{
			min = arr[i];
			index = i;
		}

	return index;
}


int find_index_max(double* arr, int n)
{
	int index = 0;
	double max = arr[index];

	for (int i = 0; i < n; i++)
		if (arr[i] > max)
		{
			max = arr[i];
			index = i;
		}

	return index;
}


void swap_min_max(double* arr, int n)
{
	int
		index1 = find_index_min(arr, n),
		index2 = find_index_max(arr, n);

	double tmp = arr[index1];
	arr[index1] = arr[index2];
	arr[index2] = tmp;
}


void swap(int* first, int* second)
{
	int tmp = *first;
	*first = *second;
	*second = tmp;
}


void bubble_sort(int arr[], int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = 1; j < n - i; j++)
		{
			if (arr[j - 1] > arr[j])
			{
				swap(&arr[j - 1], &arr[j]);
			}
		}
	}
}


// Поиск индекса числа num в массиве arr
int binary_search(int arr[], int left, int right, int num)
{
	if (right >= left)
	{
		int mid = (left + right) / 2;

		if (arr[mid] == num)
			return mid;

		if (arr[mid] > num)
			return binary_search(arr, left, mid - 1, num);

		return binary_search(arr, mid + 1, right, num);

	}

	return -1;
}
