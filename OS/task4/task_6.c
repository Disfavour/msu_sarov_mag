#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CNT 100


int left, right, right_bot, target, cnt = 0;
int fd[3][2];
int pipe_read, pipe_write;
char* name;
int k, status;


void SigHndlr(int s)
{
	if (cnt <= MAX_CNT - 2)
	{
		read(fd[pipe_read][0], &cnt, sizeof(int));
		printf("%d %s pid=%d\n", cnt, name, getpid());
		cnt++;

		if (cnt <= MAX_CNT)
		{
			write(fd[pipe_write][1], &cnt, sizeof(int));
			kill(target, SIGUSR1);
		}
		else
		{
			kill(right_bot, SIGUSR1);
		}
		
	}
	else
	{
		if (getpid() == right_bot)
		{
			close(fd[pipe_read][0]);
			close(fd[pipe_write][1]);
			kill(left, SIGUSR1);
			exit(0);
		}
		else // if (getpid() == left)
		{
			close(fd[pipe_read][0]);
			close(fd[pipe_write][1]);
			exit(0);
		}
	}
}


int main(int argc, char **argv)
{
	signal(SIGUSR1, SigHndlr);

	for (int i = 0; i < 3; ++i)
	{
		pipe(fd[i]);
	}

	pipe_read = 2;
	pipe_write = 0;

	// left
	if (!(target = fork()))
	{
		close(fd[0][0]);

		close(fd[1][0]);
		close(fd[1][1]);

		close(fd[2][1]);

		left = getpid();

		write(fd[pipe_write][1], &left, sizeof(int));
		read(fd[pipe_read][0], &right_bot, sizeof(int));

		write(fd[pipe_write][1], &right_bot, sizeof(int));
		read(fd[pipe_read][0], &right, sizeof(int));

		target = right;

		char* str = "Pang";
		name = str;

		// start from left
		// write(fd[pipe_write][1], &cnt, sizeof(int));
		// kill(target, SIGUSR1);

		while (1);
	}

	pipe_read = 0;
	pipe_write = 1;

	// right
	if (!(target = fork()))
	{
		// right bot
		if (!(target = fork()))
		{
			pipe_read++;
			pipe_write++;

			close(fd[0][0]);
			close(fd[0][1]);

			close(fd[1][1]);

			close(fd[2][0]);

			right_bot = getpid();

			write(fd[pipe_write][1], &right_bot, sizeof(int));
			read(fd[pipe_read][0], &right, sizeof(int));

			write(fd[pipe_write][1], &right, sizeof(int));
			read(fd[pipe_read][0], &left, sizeof(int));

			target = left;

			char* str = "Pong";
			name = str;

			// start from right_bot
			// sleep(1);
			// write(fd[pipe_write][1], &cnt, sizeof(int));
			// kill(target, SIGUSR1);

			while (1);
		}

		close(fd[0][1]);

		close(fd[1][0]);

		close(fd[2][0]);
		close(fd[2][1]);

		right = getpid();

		write(fd[pipe_write][1], &right, sizeof(int));
		read(fd[pipe_read][0], &left, sizeof(int));

		write(fd[pipe_write][1], &left, sizeof(int));
		read(fd[pipe_read][0], &right_bot, sizeof(int));

		target = right_bot;

		char* str = "Ping";
		name = str;

		// start from right
		// write(fd[pipe_write][1], &cnt, sizeof(int));
		// kill(target, SIGUSR1);

		k = wait(&status);

		if (WIFEXITED(status))
		{
			printf("right (%d) has waited %d status %d\n", getpid(), k, WEXITSTATUS(status));
		}

		close(fd[pipe_read][0]);
		close(fd[pipe_write][1]);

		exit(0);
	}

	// start from master
	sleep(1);
	write(fd[pipe_read][1], &cnt, sizeof(int));
	kill(target, SIGUSR1);
	
	for (int i = 0; i < 3; ++i)
	{
		close(fd[i][0]);
		close(fd[i][1]);
	}

	for (int i = 0; i < 2; ++i)
	{
		k = wait(&status);

		if (WIFEXITED(status))
		{
			printf("master (%d) has waited %d status %d\n", getpid(), k, WEXITSTATUS(status));
		}
	}

	return 0;
}
