#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Use: %s number\n", argv[0]);
        return 1;
    }

    sleep(1);

    int n, number = atoi(argv[1]);

    int fd = open("fifo", O_RDWR);

	n = write(fd, &number, sizeof(int));

    printf("Client have written %d (write return %d)\n", number, n);

	close(fd);

    return 0;
}
