#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "threads_utils.h"
#include "ring_buffer.h"

// static inline int read_file(char *buffer, const size_t buffer_size, char *filename);

void *reader()
{   
    size_t buff_size = sizeof(char) * MAX_LINE_LENGTH * (num_of_cores + 1);
    char *buffer = malloc(buff_size);
    pthread_cleanup_push(clean_up_func, buffer);

    while (1)
    {
        if (read_file(buffer, buff_size, STATS_FILENAME) == ENOENT)
        {
            free(buffer);
            break;
        }
        th_rb_push_back(&th_rb_for_reading, buffer);
        if_job_done[READER_TH_NUM]++;
        sleep(1);
    }

    pthread_cleanup_pop(1);

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
