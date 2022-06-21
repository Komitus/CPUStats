#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include "threads_utils.h"
#include "ring_buffer.h"

extern int errno;
//static inline int read_file(char *buffer, const size_t buffer_size, char *filename);

void *reader(void *__thread_struct)
{
    thread_struct *ths = (thread_struct *)__thread_struct;
    printf("Number of cores: %d\n", ths->num_of_cores - 1);

    int *err_num = malloc(sizeof(int));
    *err_num = 0;
    size_t buff_size = sizeof(char) * MAX_LINE_LENGTH * ths->num_of_cores;
    char *buffer = malloc(buff_size);

    printf("buff_size: %ld \n", buff_size);

    while (1)
    {   
        *err_num = read_file(buffer, buff_size, STATS_FILENAME);

        if (*err_num == ENOENT)
        {
            break;
        }

        th_rb_push_back(ths->th_rb, buffer);
        sleep(1);
    }

    return (void *)err_num;
}

int read_file(char *buffer, const size_t buffer_size, char *filename)
{
    FILE *in_file = fopen(filename, "r");

    if (in_file == NULL)
        return ENOENT;

    fread(buffer, sizeof(char), buffer_size, in_file);
    fclose(in_file);

    return 0;
}

unsigned short get_num_of_cores()
{
    unsigned short num_of_cores = 0;
    char buffer[MAX_LINE_LENGTH];

    FILE *in_file = fopen(STATS_FILENAME, "r");

    if (in_file == NULL)
        err(EXIT_FAILURE, "Error code: %d", ENOENT);

    // calculating number of cores
    while (fgets(buffer, sizeof buffer, in_file))
        if (strncmp(buffer, "cpu", 3) == 0)
            num_of_cores++;
    // end calculating
    fclose(in_file);

    return num_of_cores;
}
