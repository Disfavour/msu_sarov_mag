#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

#define NMAX 256


int main()
{
	key_t key;
	int semid, shmid;
	struct sembuf sops;
	char* shmaddr;
	char str[NMAX];

	key = ftok("p1.c", 'S');

	semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
	shmid = shmget(key, NMAX, 0666 | IPC_CREAT | IPC_EXCL);

	shmaddr = shmat(shmid, NULL, 0);
	semctl(semid, 0, SETVAL, 0);

	sops.sem_num = 0;
	sops.sem_flg = 0;

	do
	{
		printf("Enter string: ");

		if (fgets(str, NMAX, stdin) == NULL)
		{
			strcpy(str, "Q");
		}

		strcpy(shmaddr, str);

		sops.sem_op = 3;
		semop(semid, &sops, 1);
		sops.sem_op = 0;
		semop(semid, &sops, 1);
	}
	while (str[0] != 'Q');

	shmdt(shmaddr);
	shmctl(shmid, IPC_RMID, NULL);
	semctl(semid, 0, IPC_RMID, 0);

	return 0;
}
