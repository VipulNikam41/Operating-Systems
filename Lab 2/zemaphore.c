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


void zem_init(zem_t *s, int value) 
{
	s->count = value;

	pthread_cond_init(&s->go_sleepers,NULL);
	pthread_mutex_init(&s->accesslock,NULL);
}

void zem_down(zem_t *s) 
{
	pthread_mutex_lock(&s->accesslock);
	while(s->count <= 0)
	{
		pthread_cond_wait(&s->go_sleepers,&s->accesslock);
	}
	s->count -= 1;
	pthread_mutex_unlock(&s->accesslock);
}

void zem_up(zem_t *s) 
{
	pthread_mutex_lock(&s->accesslock);
	s->count += 1;
	pthread_cond_signal(&s->go_sleepers);
	pthread_mutex_unlock(&s->accesslock);
}
