#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "threads_utils.h"
#include "ring_buffer.h"

// static inline int read_file(char *buffer, const size_t buffer_size, char *filename);

void *reader(void *arg)
{
    ThreadStruct *thread_struct = (ThreadStruct *)arg;
    size_t buff_size = sizeof(char) * MAX_LINE_LENGTH * (thread_struct->num_of_cores + 1);
    ThreadedRingBuffer *th_rb_for_sending = thread_struct->th_rbs[0];
    char *buffer = malloc(buff_size);

    while (1)
    {
        if (read_file(buffer, buff_size, STATS_FILENAME) == ENOENT)
        {
            free(buffer);
            break;
        }
        th_rb_push_back(th_rb_for_sending, buffer);
        sleep(1);
    }

    th_rb_free(th_rb_for_sending);
    free(buffer);

    return NULL;
}

int read_file(char *buffer, const size_t buffer_size, char *filename)
{
    FILE *in_file = fopen(filename, "r");

    if (in_file == NULL)
        return ENOENT;

    fread(buffer, sizeof(char), buffer_size-1, in_file);
    // i want \0 at the end (buffer[buff_size-1])
    fclose(in_file);

    return 0;
}

int get_num_of_cores()
{
    unsigned short num_of_cores = 0;
    char buffer[MAX_LINE_LENGTH];

    FILE *in_file = fopen(STATS_FILENAME, "r");

    if (in_file == NULL)
    {
        return -1;
    }

    // calculating number of cores
    while (fgets(buffer, sizeof(char) * MAX_LINE_LENGTH-1, in_file))
    {   
        if (strncmp(buffer, "cpu", 3) == 0)
        {
            num_of_cores++;
        }
    }

    // end calculating
    num_of_cores--; // don't count general
    fclose(in_file);

    return num_of_cores;
}
