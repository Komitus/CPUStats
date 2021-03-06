#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "threads_utils.h"
#include "ring_buffer.h"

void *reader(void *arg)
{
    MY_LOG("READER STARTED");
    const unsigned char thread_num = *((unsigned char *)arg);
    const size_t buff_size = sizeof(char) * MAX_LINE_LENGTH * (g_shared_data.num_of_cores + 1);
    char *buffer = malloc(buff_size);

    while (atomic_load(&g_shared_data.running[thread_num]))
    {
        if (read_file(buffer, buff_size, STATS_FILENAME) == ENOENT)
        {
            free(buffer);
            break;
        }
        th_rb_push_back(&g_shared_data.th_rb_for_raw_data, buffer);
        MY_LOG("READER PUSHED");
        atomic_store(&g_shared_data.job_done[thread_num], 1);

        sleep(1);
    }

    free(buffer);
    MY_LOG("EXIT READER");

    return NULL;
}

int read_file(char *buffer, const size_t buffer_size, char *filename)
{
    FILE *in_file = fopen(filename, "r");

    if (in_file == NULL)
        return ENOENT;

    fread(buffer, sizeof(char), buffer_size - 1, in_file);
    buffer[buffer_size - 1] = '\0';
    // i want \0 at the end (buffer[buff_size-1])
    fclose(in_file);

    return 0;
}

int get_num_of_cores(void)
{
    unsigned short cores = 0;
    char buffer[MAX_LINE_LENGTH];

    FILE *in_file = fopen(STATS_FILENAME, "r");

    if (in_file == NULL)
    {
        return -1;
    }

    // calculating number of cores
    while (fgets(buffer, sizeof(char) * MAX_LINE_LENGTH - 1, in_file))
    {
        if (strncmp(buffer, "cpu", 3) == 0)
        {
            cores++;
        }
    }

    // end calculating
    cores--; // don't count general
    fclose(in_file);

    return cores;
}
