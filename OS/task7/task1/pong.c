#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define NMAX 256


char* shmaddr;


void handler(int i)
{
	shmdt(shmaddr);
	printf("pong exited normally\n");
	exit(0);
}


int main()
{
	signal(SIGINT, handler);

	key_t key;
	int semid, shmid;
	struct sembuf sops[2];
	char* shmaddr;
	char str[NMAX];

	char* me = "pong";

	key = ftok("ping.c", 'S');

	semid = semget(key, 2, 0666);
	shmid = shmget(key, NMAX, 0666);

	shmaddr = shmat(shmid, NULL, 0);

	sops[0].sem_num = 0;
	sops[0].sem_op = 1;
	sops[0].sem_flg = 0;

	sops[1].sem_num = 1;
	sops[1].sem_op = -1;
	sops[1].sem_flg = 0;

	do
	{
		semop(semid, sops+1, 1);

		printf("%s\n", shmaddr);

		strcpy(shmaddr, me);

		sleep(1);

		semop(semid, sops, 1);
	}
	while (1);

	return 0;
}
