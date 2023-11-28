#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5

int semid;

int philosophers[N];


void handler(int i)
{
    for (int i = 0; i < N; i++)
    {
        kill(philosophers[0], SIGTERM);
    }

    for (int i = 0; i < N; i++)
	{
		wait(NULL);
	}

    for (int i = 0; i < N; i++)
    {
        semctl(semid, i, IPC_RMID, NULL);
    }

	printf("\nExited normally\n");

	exit(0);
}

void wait()
{
    sleep(1 + rand() % 4);
}

void think(int id)
{
    printf("Philosopher %d is thinking\n", id);
    wait();
}

void take_forks(int id, struct sembuf* take_fork)
{
    printf("Philosopher %d trying to take forks\n", id);

    int 
        left_fork = id,
        right_fork = id + 1 < N ? id + 1 : 0;

    struct sembuf chosen_forks[2] = {take_fork[left_fork], take_fork[right_fork]};

    semop(semid, chosen_forks, 2);
}

void eat(int id)
{
    printf("Philosopher %d is eating\n", id);
    wait();
}

void put_forks(int id, struct sembuf* put_fork)
{
    printf("Philosopher %d put forks\n", id);

    int 
        left_fork = id,
        right_fork = id + 1 < N ? id + 1 : 0;

    struct sembuf chosen_forks[2] = {put_fork[left_fork], put_fork[right_fork]};

    semop(semid, chosen_forks, 2);
}

int main()
{
	signal(SIGINT, handler);

	key_t key = ftok("philosophers.c", 'S');

    semid = semget(key, N, 0666 | IPC_CREAT | IPC_EXCL);

    for (int i = 0; i < N; i++)
    {
        semctl(semid, i, SETVAL, 1);
    }

    struct sembuf take_fork[N], put_fork[N];

    for (int i = 0; i < N; i++)
    {
        take_fork[i].sem_num = i;
        take_fork[i].sem_op = -1;
        take_fork[i].sem_flg = 0;

        put_fork[i].sem_num = i;
        put_fork[i].sem_op = 1;
        put_fork[i].sem_flg = 0;
    }

    for (int i = 0; i < N; i++)
    {
        if (!(philosophers[i] = fork()))
        {
            signal(SIGINT, SIG_DFL);
            int id = i;

            while (1)
            {
                think(id);
                take_forks(id, take_fork);
                eat(id);
                put_forks(id, put_fork);
            }
        }
    }

    while (1);

    return 0;
}
