#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
    //	Write the code for initializing your read-write lock.
    rw->readerCount = rw->writersWaiting = 0;
    rw->writerActive = rw->fls;

    rw->condVariable = PTHREAD_COND_INITIALIZER;
    rw->lock = PTHREAD_MUTEX_INITIALIZER;
}

void ReaderLock(struct read_write_lock * rw)
{
	//	Write the code for aquiring read-write lock by the reader.
	pthread_mutex_lock(&rw->lock);

	while (rw->writersWaiting > 0 || rw->writerActive != rw->fls)
	{
		pthread_cond_wait(&rw->condVariable, &rw->lock);
	}
	
	rw->readerCount ++;

	pthread_mutex_unlock(&rw->lock);
}

void ReaderUnlock(struct read_write_lock * rw)
{
	//	Write the code for releasing read-write lock by the reader.
	pthread_mutex_lock(&rw->lock);

	rw->readerCount --;

	if (rw->readerCount == 0)
	{
		pthread_cond_broadcast(&rw->condVariable);
	}
	
	pthread_mutex_unlock(&rw->lock);
}

void WriterLock(struct read_write_lock * rw)
{
	//	Write the code for aquiring read-write lock by the writer.
	pthread_mutex_lock(&rw->lock);

	rw->writersWaiting ++;	

	while (rw->readerCount > 0 || rw->writerActive != rw->fls)
	{
		pthread_cond_wait(&rw->condVariable, &rw->lock);
	}
	
	rw->writersWaiting --;
	rw->writerActive = rw->tru;

	pthread_mutex_unlock(&rw->lock);
}

void WriterUnlock(struct read_write_lock * rw)
{
	//	Write the code for releasing read-write lock by the writer.
	pthread_mutex_lock(&rw->lock);	

	rw->writerActive = rw->fls;

	pthread_cond_broadcast(&rw->condVariable);	
	pthread_mutex_unlock(&rw->lock);
}