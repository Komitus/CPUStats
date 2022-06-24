#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "threads_utils.h"


void *printer()
{
    char *buffer = malloc(sizeof(char) * PRINT_LINE_LENGTH * (num_of_cores+1));
    pthread_cleanup_push(clean_up_func, buffer);
    double received[num_of_cores+1];

    while (1)
    {   
        th_rb_pop_front(&th_rb_for_printing, received);
        sprintf(buffer, "\nAverage: %3.2lf %%\n", received[num_of_cores]);

        for (int i = 0; i < num_of_cores; i++)
        {
            sprintf(buffer + strlen(buffer), "Core[%d]: %3.2lf %%\n", i, received[i]);
        }
        fprintf(stdout, "%s", buffer);
        if_job_done[PRINTER_TH_NUM]++;
        sleep(1);
    }

    pthread_cleanup_pop(1);

    return NULL;
}