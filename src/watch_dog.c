#include "threads_utils.h"
#include <stdatomic.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *watch_dog(void *arg)
{
    pthread_t *threads_ids = (pthread_t *)arg;
    static const char *THREAD_STRING[] = {
        "reader", "analyst", "printer"};

    clock_t t[NUM_OF_THREADS];
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        t[i] = clock();
    }

    while (1)
    {

        for (int i = 0; i < NUM_OF_THREADS; i++)
        {
            if (if_job_done[i] > 0)
            {
                if_job_done[i]--;
                t[i] = clock();
            }
            else
            {
                if (clock() - t[i] > 2 * CLOCKS_PER_SEC)
                {
                    for (int i = 0; i < NUM_OF_THREADS; i++)
                    {
                        // printf("Thread id %lu\n", threads_ids[i]);
                        pthread_cancel(threads_ids[i]);
                    }
                    printf("WATCH_DOG: threads cancelled, principal: %s\n", THREAD_STRING[i]);
                    return NULL;
                }
            }
        }
    }

    return NULL;
}

void clean_up_func(void *arg)
{
    char *buffer = (char *)arg;
    if (buffer)
    {
        free(buffer);
        //printf("BUFF CLEANED\n");
    }
}
