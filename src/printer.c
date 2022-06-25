#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "threads_utils.h"

void *printer(void *arg)
{
    const unsigned char thread_num = *((unsigned char *)arg);
    printf("PRINTER THREAD NUM: %hhu\n", thread_num);

    const unsigned short num_of_cores = g_shared_data.num_of_cores;
    char *buffer = malloc(sizeof(char) * PRINT_LINE_LENGTH * (unsigned long)(num_of_cores + 1));
    double *received = malloc(sizeof(*received) * (num_of_cores + 1));

    while (atomic_load(&g_shared_data.running[thread_num]))
    {
        if (th_rb_pop_front(&g_shared_data.th_rb_for_calculated_data, received,
                            &g_shared_data.running[thread_num]) != 0)
        {
            break;
        }
        sprintf(buffer, "\nAverage: %3.2lf %%\n", received[num_of_cores]);
        for (int i = 0; i < num_of_cores; i++)
        {
            sprintf(buffer + strlen(buffer), "Core[%d]: %3.2lf %%\n", i, received[i]);
        }
        fprintf(stdout, "%s", buffer);
        atomic_store(&g_shared_data.job_done[thread_num], 1);
        sleep(1);
    }

    free(buffer);
    free(received);

    printf("EXITED PRINTER\n");
    return NULL;
}
