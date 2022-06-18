#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

extern int errno;

void *reader()
{
    printf("Number of cores: %d\n", num_of_cores - 1);

    int *err_num = malloc(sizeof(int));
    *err_num = 0;
    char buffer[MAX_LINE_LENGTH];

    FILE *in_file = fopen("/proc/stat", "r");

    if (in_file == NULL)
    {
        *err_num = errno;
        return (void *)err_num;
    }

    CoreStats *cpu_stats = malloc(sizeof *cpu_stats * num_of_cores);

    while (1)
    {
        rewind(in_file);
        for (unsigned short i = 0; i < num_of_cores && fgets(buffer, sizeof buffer, in_file); i++)
        {
            if (sscanf(buffer, "%s %lu %lu %lu %lu %lu %lu %lu", cpu_stats[i].core_num,
                       &cpu_stats[i].user, &cpu_stats[i].nice, &cpu_stats[i].system,
                       &cpu_stats[i].idle, &cpu_stats[i].iowait, &cpu_stats[i].irq,
                       &cpu_stats[i].softirq) != NUM_OF_REQ_FIELDS)
            {
                *err_num = EIO;
                return err_num;
            }
        }
        sleep(1);
    }

    return (void *)err;
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