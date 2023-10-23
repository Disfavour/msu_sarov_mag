#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


int main(int argc, char** argv)
{
	execl("./hello_world", "hello_world", " 1", (char*)0);

	return 0;
}
