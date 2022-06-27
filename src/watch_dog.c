#include <stdatomic.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "threads_utils.h"

void *watch_dog(void *arg)
{
    const unsigned char thread_num = *((unsigned char *)arg);
    struct timespec ts;
    printf("WATCHDOG THREAD NUM: %hhu\n", thread_num);

    ts.tv_nsec = 0;
    ts.tv_sec = 2;


    // mutex and cond version takes less resources (cpu % 18 contrary to previous 27%)
    while (atomic_load(&g_shared_data.running[thread_num]))
    {
        pthread_mutex_lock(&g_shared_data.time_mutex);
        pthread_cond_timedwait(&g_shared_data.time_cond,
                               &g_shared_data.time_mutex, &ts);

        // printf("WATCH: %ld\n", g_shared_data.last_time_active);
        if (clock() - g_shared_data.last_time_active > ts.tv_sec * CLOCKS_PER_SEC)
        {
            pthread_mutex_unlock(&g_shared_data.time_mutex);
            break;
        }
        pthread_mutex_unlock(&g_shared_data.time_mutex);
    }

    stop_all_threads();
    printf("WATCH_DOG: threads stopped\n");

    return NULL;
}
