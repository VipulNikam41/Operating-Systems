#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <fcntl.h> 
#include <sys/shm.h> 
#include <semaphore.h>

int item_to_produce,item_to_fulfill, curr_buf_size;
int total_items, max_buf_size, num_workers, num_masters;
int *buffer;

sem_t *item_sema;

pthread_mutex_t _mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t _mutex2 = PTHREAD_MUTEX_INITIALIZER;

void print_produced(int num, int master) 
{
    printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker)
{
    printf("Consumed %d by worker %d\n", num, worker);
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
    int thread_id = *((int *)data);
    while(1)
    {
        if(item_to_produce >= total_items) 
        {
            break;
        }

        pthread_mutex_lock(&_mutex1);
        buffer[curr_buf_size++] = item_to_produce;
        print_produced(item_to_produce, thread_id);
        item_to_produce++;
        sem_post(item_sema);
        pthread_mutex_unlock(&_mutex1);
    }
    return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *comsumed_requests_loop(void *data)
{
    int thread_id = *((int *)data);
    while(1)
    {
        if(item_to_fulfill >= total_items) 
        {
            break;
        }

        sem_wait(item_sema);
        pthread_mutex_lock(&_mutex2);
        int temp = buffer[item_to_fulfill++];
        print_consumed(item_to_fulfill-1, thread_id);
        pthread_mutex_unlock(&_mutex2);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int *master_thread_id;
    int *worker_thread_id;

    pthread_t *master_thread;
    pthread_t *worker_thread;

    item_sema = sem_open("/itemsema", O_CREAT, 0666, 1);

    item_to_produce = 0;
    item_to_fulfill = 0;
    curr_buf_size = 0;

    int i;

    if (argc < 5) 
    {
        printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
        exit(1);
    }
    else 
    {
        total_items = atoi(argv[1]);    //M
        max_buf_size = atoi(argv[2]);   //N
        num_workers = atoi(argv[3]);    //C
        num_masters = atoi(argv[4]);    //p
    }

    buffer = (int *)malloc (sizeof(int) * max_buf_size);

    //create master producer threads
    master_thread_id = (int *)malloc(sizeof(int) * num_masters);
    master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);

    for (i = 0; i < num_masters; i++)
    {
        master_thread_id[i] = i;
    }

    //create worker consumer threads
    worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
    worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);

    for (i = 0; i < num_workers; i++)
    {
        worker_thread_id[i] = i;
    }


    for (i = 0; i < num_masters; i++)
    {
        pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
    }


    for (i = 0; i < num_workers; i++)
    {
        pthread_create(&worker_thread[i], NULL, comsumed_requests_loop, (void *)&worker_thread_id[i]);
    }

    //wait for all threads to complete
    for (i = 0; i < num_masters; i++)
    {
        pthread_join(master_thread[i], NULL);
        printf("master %d joined\n", i);
    }

    for (i = 0; i < num_workers; i++)
    {
        pthread_join(worker_thread[i], NULL);
        printf("worker %d joined\n", i);
    }

    /*----Deallocating Buffers---------------------*/
    free(buffer);
    free(master_thread_id);
    free(master_thread);
    free(worker_thread_id);
    free(worker_thread);

    return 0;
}