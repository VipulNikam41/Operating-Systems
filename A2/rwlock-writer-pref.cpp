#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
    //	Write the code for initializing your read-write lock.
    rw->readerCount = 0;
	rw->writersWaiting = 0;
    rw->writerActive = false;
    rw->condVariable = PTHREAD_COND_INITIALIZER;
    rw->lock = PTHREAD_MUTEX_INITIALIZER;
}

void ReaderLock(struct read_write_lock * rw)
{
	//	Write the code for aquiring read-write lock by the reader.
	pthread_mutex_lock(&rw->lock);
	while (rw->writersWaiting > 0 || rw->writerActive)
		pthread_cond_wait(&rw->condVariable, &rw->lock);
	
	rw->readerCount += 1;
	pthread_mutex_unlock(&rw->lock);
}

void ReaderUnlock(struct read_write_lock * rw)
{
	//	Write the code for releasing read-write lock by the reader.
	pthread_mutex_lock(&rw->lock);
	rw->readerCount -= 1;
	if (rw->readerCount == 0)
		pthread_cond_broadcast(&rw->condVariable);
	
	pthread_mutex_unlock(&rw->lock);
}

void WriterLock(struct read_write_lock * rw)
{
	//	Write the code for aquiring read-write lock by the writer.
	pthread_mutex_lock(&rw->lock);
	rw->writersWaiting += 1;	
	while (rw->readerCount > 0 || rw->writerActive)
		pthread_cond_wait(&rw->condVariable, &rw->lock);
	
	rw->writersWaiting -= 1;
	rw->writerActive = true;
	pthread_mutex_unlock(&rw->lock);
}

void WriterUnlock(struct read_write_lock * rw)
{
	//	Write the code for releasing read-write lock by the writer.
	pthread_mutex_lock(&rw->lock);	
	rw->writerActive = false;
	pthread_cond_broadcast(&rw->condVariable);	
	pthread_mutex_unlock(&rw->lock);
}
