#include <stdatomic.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "threads_utils.h"

void *watch_dog(void *arg)
{   
    const unsigned char thread_num = *((unsigned char *)arg);
    printf("WATCHDOG THREAD NUM: %hhu\n", thread_num);
    
    clock_t *t = malloc(sizeof(*t) * NUM_OF_OBLIGATORY_THREADS);

    for (int i = 0; i < NUM_OF_OBLIGATORY_THREADS; i++)
    {
        t[i] = clock();
    }

    while (atomic_load(&g_shared_data.running[thread_num]))
    {

        for (int i = 0; i < NUM_OF_OBLIGATORY_THREADS; i++)
        {
            if (atomic_load(&g_shared_data.job_done[i]))
            {
                atomic_store(&g_shared_data.job_done[i], 0);
                t[i] = clock();
            }
            else
            {
                if (clock() - t[i] > 100 * CLOCKS_PER_SEC)
                {   
                    printf("WATCH_DOG: threads cancelled, principal: %s\n", THREAD_STRING[i]);
                    stop_all_threads(); 
                    free(t);
                    return NULL;
                }
            }
        }
    }
    
    free(t);
    return NULL;
}
