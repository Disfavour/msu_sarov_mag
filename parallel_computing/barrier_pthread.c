#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>


struct some_data
{
	int rank, size;
	sem_t** sems;
} typedef parameters;


int get_layers(int size)
{
	const float x = log2(size);
	return (x - (int)x == 0) ? x : ceil(x);
}


void* barrier(void *data)
{
	parameters* params = data;

	const int
		rank = params->rank,
		size = params->size;

	sem_t** sems = params->sems;

	const int layers = get_layers(size);

	for (int i = 1; i <= layers; ++i)
	{
		const int
			k = 1 << i,
			offset = k >> 1;

		for (int j = 0; j*k < size; ++j)
		{
			const int
				receiver = j*k,
				sender = receiver + offset;

			if (sender < size)
			{
				if (rank == receiver)
				{
					sem_post(sems[i-1]+sender);
					sem_wait(sems[i-1]+receiver);
				}
				else if (rank == sender)
				{
					sem_post(sems[i-1]+receiver);
					sem_wait(sems[i-1]+sender);
				}
			}
		}
	}

	for (int i = layers; i >= 1; --i)
	{
		const int
			k = 1 << i,
			offset = k >> 1;

		for (int j = 0; j*k < size; ++j)
		{
			const int
				sender = j*k,
				receiver = sender + offset;

			if (receiver < size)
			{
				if (rank == receiver)
				{
					sem_post(sems[i-1]+sender);
					sem_wait(sems[i-1]+receiver);
				}
				else if (rank == sender)
				{
					sem_post(sems[i-1]+receiver);
					sem_wait(sems[i-1]+sender);
				}
			}
		}
	}
}


void* work(void *data)
{
	parameters* params = data;

	const int
		rank = params->rank,
		size = params->size;

	if (rank == 1)
	{
		sleep(2);
	}
	else if (rank && !(rank % 3))
	{
		sleep(4);
	}

	printf("Start of the barrier %d/%d\n", rank, size);

	barrier(data);

	printf("End of the barrier %d/%d\n", rank, size);
}


int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Use: %s num_threads\n", argv[0]);
		return 1;
	}
	
	const int nthreads = atoi(argv[1]);

	pthread_t* thr = malloc(nthreads*sizeof(pthread_t));

	const int layers = get_layers(nthreads);

	sem_t** sems = malloc(layers*sizeof(sem_t*));
	for (int i = 0; i < layers; ++i)
	{
		sems[i] = malloc(nthreads*sizeof(sem_t));
		for (int j = 0; j < nthreads; ++j)
		{
			sem_init(sems[i]+j, 0, 0);
		}
	}

	parameters* params = malloc(nthreads*sizeof(parameters));

	for (int i = 0; i < nthreads; ++i)
	{
		params[i].rank = i;
		params[i].size = nthreads;
		params[i].sems = sems;

		pthread_create(thr+i, NULL, work, params+i);
	}

	for (int i = 0; i < nthreads; ++i)
	{
		pthread_join(thr[i], NULL);
	}

	free(params);

	for (int i = 0; i < layers; ++i)
	{
		for (int j = 0; j < nthreads; ++j)
		{
			sem_destroy(sems[i]+j);
		}
		free(sems[i]);
	}
	free(sems);

	free(thr);

	return 0;
}
