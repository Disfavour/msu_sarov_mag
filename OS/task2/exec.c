#include <stdio.h>
#include <unistd.h>


int main()
{
	execl("/bin/gcc", "gcc", "1.c", "-o", "1.exe", "-lm", (char*)0);

	return 0;
}
