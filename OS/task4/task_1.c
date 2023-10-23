#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		return 0;
	}

	if (argc == 2)
	{
		execlp(argv[1], argv[1], NULL);
	}

	int fd[2];

	pipe(fd);

	int
		pipe_read = fd[0],
		pipe_write = fd[1];

	if (!fork())
	{
		close(pipe_read);
		dup2(pipe_write, 1);
		close(pipe_write);
		execlp(argv[1], argv[1], NULL);
	}

	close(pipe_write);

	int old_pipe_read = pipe_read;

	for (int i = 2; i < argc-1; i++)
	{
		pipe(fd);
		pipe_read=fd[0];
		pipe_write=fd[1];

		if (!fork())
		{
			close(pipe_read);
			dup2(old_pipe_read, 0);
			close(old_pipe_read);
			dup2(pipe_write, 1);
			close(pipe_write);
			execlp(argv[i], argv[i], NULL);
		}

		close(old_pipe_read);
		close(pipe_write);
		old_pipe_read = pipe_read;
	}

	if (!fork())
	{
		dup2(old_pipe_read, 0);
		close(old_pipe_read);
		execlp(argv[argc-1], argv[argc-1], NULL);
	}

	close(old_pipe_read);

	for (int i = 1; i < argc; i++)
	{
		wait(NULL);
	}

	return 0;
}
