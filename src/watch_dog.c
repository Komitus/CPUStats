#include <stdatomic.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "threads_utils.h"

void *watch_dog(void *arg)
{   
    MY_LOG("WATCHDOG STARTED");
    const unsigned char thread_num = *((unsigned char *)arg);
    const unsigned int secs_to_wait = 2;

    while (atomic_load(&g_shared_data.running[thread_num]))
    {   
        sleep(secs_to_wait);
        for (int i = 0; i < NUM_OF_OBLIGATORY_THREADS; i++)
        {
            if (atomic_load(&g_shared_data.job_done[i]))
            {
                atomic_store(&g_shared_data.job_done[i], 0);
            }
            else
            {
                stop_all_threads(); 
                MY_LOG("WATCH_DOG: threads cancelled\n");
                fprintf(stderr, "WATCH_DOG: threads cancelled, principal: %s\n", THREAD_STRING[i]);
                return NULL;
            }
        }
    }

    return NULL;
}
