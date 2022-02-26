#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
// #include "zemaphore.h"

// zemaphore.h // zemaphore.h // zemaphore.h // zemaphore.h
typedef struct zemaphore 
{
	int count;
	
	pthread_mutex_t accesslock;
	pthread_cond_t go_sleepers;
}zem_t;

void zem_init(zem_t *, int);
void zem_up(zem_t *);
void zem_down(zem_t *);
// zemaphore.h // zemaphore.h // zemaphore.h // zemaphore.h

int NUM_THREADS = 2;
int NUM_ITER = 10;

zem_t * zemlist;
int count =0;

void *justprint(void *data)
{
	int thread_id = *((int *)data);

	int i=0; 
	while(i < NUM_ITER)
	{
		zem_down(&zemlist[thread_id]);

		printf("This is thread %d\n", thread_id);

		count = (thread_id+1) % NUM_THREADS;
		zem_up(&zemlist[count]);

		i++;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 3)
	{
		NUM_THREADS = atoi(argv[1]);
		// printf("%d\n", NUM_THREADS);

		NUM_ITER = atoi(argv[2]);
		// printf("%d\n",NUM_ITER );
	}

	NUM_THREADS++;

	pthread_t mythreads[NUM_THREADS];

	int mythread_id[NUM_THREADS];

	zemlist = (zem_t *)malloc(sizeof(zem_t) * NUM_THREADS);

	int i = 0; 
	while(i < NUM_THREADS)
	{
		zem_init(&zemlist[i], 0);

		mythread_id[i] = i;
		pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);

		i++;
	}

	zem_up(&zemlist[0]);

	i =0; 
	while(i < NUM_THREADS)
	{
		pthread_join(mythreads[i], NULL);

		i++;
	}
	
	return 0;
}
