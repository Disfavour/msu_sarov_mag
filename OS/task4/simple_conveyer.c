#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		return -1;
	}

	pipe(fd);

	if(fork())
	{
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);
		execl("/usr/bin/yes", "yes", NULL);
	}

	dup2(fd[0],0);
	close(fd[0]);
	close(fd[1]);
	execl("/usr/bin/head", "head", NULL);

	return 0;
}
