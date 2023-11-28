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


int semid, shmid;
char* shmaddr;


void handler(int i)
{
	shmdt(shmaddr);
	shmctl(shmid, IPC_RMID, NULL);
	semctl(semid, 0, IPC_RMID, NULL);
	semctl(semid, 1, IPC_RMID, NULL);
	printf("ping exited normally\n");
	exit(0);
}


int main()
{
	signal(SIGINT, handler);
	key_t key;
	
	struct sembuf sops[2];
	
	char str[NMAX];

	char* me = "ping";

	key = ftok("ping.c", 'S');

	semid = semget(key, 2, 0666 | IPC_CREAT | IPC_EXCL);
	shmid = shmget(key, NMAX, 0666 | IPC_CREAT | IPC_EXCL);

	shmaddr = shmat(shmid, NULL, 0);
	semctl(semid, 0, SETVAL, 1);
	semctl(semid, 1, SETVAL, 0);

	sops[0].sem_num = 0;
	sops[0].sem_op = -1;
	sops[0].sem_flg = 0;

	sops[1].sem_num = 1;
	sops[1].sem_op = 1;
	sops[1].sem_flg = 0;

	strcpy(shmaddr, "pong");

	do
	{
		semop(semid, sops, 1);

		printf("%s\n", shmaddr);

		strcpy(shmaddr, me);

		sleep(1);

		semop(semid, sops+1, 1);
	}
	while (1);

	return 0;
}
