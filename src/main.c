#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <err.h>
#include <assert.h>

#include "threads_utils.h"

int num_of_cores;
atomic_int if_job_done[NUM_OF_THREADS] = {0,0,0};
atomic_int if_work[NUM_OF_THREADS] = {1,1,1};

ThreadedRingBuffer th_rb_for_reading;
ThreadedRingBuffer th_rb_for_printing;

// int argc, char *argv[]
int main()
{   
    num_of_cores = get_num_of_cores();

    if (num_of_cores <= 0 || num_of_cores > MAX_NUM_OF_CORES)
    {
        err(EXIT_FAILURE, "Cannot get proper number of cores\n");
    }
    assert (num_of_cores > 0 || num_of_cores < MAX_NUM_OF_CORES);
    printf("Number of cores: %d\n", num_of_cores);

    void* (*functions[NUM_OF_THREADS])() = {
        &reader, &analyst, &printer
    };

    th_rb_init(&th_rb_for_reading, MAX_BUFF_ENTRIES, sizeof(char) * MAX_LINE_LENGTH * (num_of_cores + 1));
    th_rb_init(&th_rb_for_printing, MAX_BUFF_ENTRIES, sizeof(double) * (num_of_cores + 1));


    pthread_t threads[NUM_OF_THREADS];
    pthread_t watch_dog_th;

    for(int i = 0; i < NUM_OF_THREADS; i++){
        if (pthread_create(&threads[i], NULL, functions[i], NULL) != 0)
        {
            fprintf(stderr, "Error while creating thread\n");
        }
        //printf("Id created: %lu\n", threads[i]);

    }


    if (pthread_create(&watch_dog_th, NULL, &watch_dog, &threads) != 0)
    {
        fprintf(stderr, "Error while creating thread\n");
    }

    if(pthread_join(watch_dog_th, NULL)){
        fprintf(stderr, "Error while joining thread\n");

    }

    th_rb_free(&th_rb_for_reading);
    th_rb_free(&th_rb_for_printing);
    
    return 0;
}
