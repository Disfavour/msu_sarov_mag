#include <stdio.h>


void ncopy(char strin[], char strout[], int* n)
{
	int i = 0;
	while (i++ < *n && *strin && (*strout++ = *strin++));
}
