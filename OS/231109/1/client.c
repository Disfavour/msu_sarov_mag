#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
	int fd;
	int pid = getpid();

	fd = open("fifo", O_RDWR);

	write(fd, &pid, sizeof(int));
	sleep(5);
	close(fd);
}