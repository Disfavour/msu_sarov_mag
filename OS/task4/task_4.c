#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


int main(int argc, char** argv)
{
	if (argc != 4)
	{
		// pr1 < f.dat > f.res 
		printf("Use: %s pr1 f.dat f.res\n", argv[0]);
		return -1;
	}

	int f_in = open(argv[2], O_RDONLY),
		f_out = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666);

	if (!fork())
	{
		dup2(f_in, 0);
		close(f_in);

		dup2(f_out, 1);
		close(f_out);
		
		execlp(argv[1], argv[1], NULL);
	}

	close(f_in);
	close(f_out);

	wait(NULL);
	
	return 0;
}
