#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "threads_utils.h"

#define SINGLE_LINE_WIDTH 16

void *printer(void *arg)
{

    ThreadStruct *thread_struct = (ThreadStruct *)arg;
    const unsigned short num_of_cores = thread_struct->num_of_cores;
    ThreadedRingBuffer *th_rb_for_receiving = thread_struct->th_rbs[0];
    double received[num_of_cores];

    char *buffer = malloc(sizeof(char) * SINGLE_LINE_WIDTH * num_of_cores);

    while (1)
    {
        sleep(1);
        th_rb_pop_front(th_rb_for_receiving, received);
        sprintf(buffer, "\nAverage: %3.2lf%%\n", received[num_of_cores]);

        for (int i = 0; i < num_of_cores; i++)
        {
            sprintf(buffer + strlen(buffer), "Core[%d]: %3.2lf%%\n", i, received[i]);
        }
        fprintf(stdout, "%s", buffer);
    }

    return NULL;
}