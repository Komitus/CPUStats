#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

extern int errno;
static inline int read_file(char *buffer, const size_t buffer_size, char *filename);
/**
 * @brief Parse raw bytes (char*) into struct with proper fileds (cpu stats)
 * @param buffer
 * @param cpu_stats
 * @param num_of_cores
 * @return int - error num
 */
static inline int parse_to_struct(char *buffer, CoreStats *cpu_stats, const unsigned short num_of_cores);

void *reader(void *_num_of_cores)
{
    unsigned short num_of_cores = *(unsigned short *)_num_of_cores;
    printf("Number of cores: %d\n", num_of_cores - 1);

    int *err_num = malloc(sizeof(int));
    *err_num = 0;
    size_t buff_size = sizeof(char) * MAX_LINE_LENGTH * num_of_cores;
    char *buffer = malloc(buff_size);
    CoreStats *cpu_stats = malloc(sizeof *cpu_stats * num_of_cores);

    printf("%ld \n", sizeof *buffer * MAX_LINE_LENGTH * num_of_cores);

    while (1)
    {
        read_file(buffer, buff_size, STATS_FILENAME);
        *err_num = parse_to_struct(buffer, cpu_stats, num_of_cores);
        sleep(1);
    }

    return (void *)err;
}

static inline int read_file(char *buffer, const size_t buffer_size, char *filename)
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

static inline int parse_to_struct(char *buffer, CoreStats *cpu_stats, const unsigned short num_of_cores)
{
    int bytes_consumed = 0;
    int bytes_now = 0;

    cpu_stats[0].core_num = -1;
    unsigned long garbage;

    if (sscanf(buffer, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu%n",
               &cpu_stats[0].user, &cpu_stats[0].nice, &cpu_stats[0].system,
               &cpu_stats[0].idle, &cpu_stats[0].iowait, &cpu_stats[0].irq,
               &cpu_stats[0].softirq,
               &garbage, &garbage, &garbage,
               &bytes_now) != 10)

    {   
        return EIO;
    }
    bytes_consumed += bytes_now+1; // +1 is for  \n 
    
    for (unsigned short i = 1; i < num_of_cores; i++)
    {   
        if(sscanf(buffer + bytes_consumed, "cpu%d %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu%n",
                    &cpu_stats[i].core_num, &cpu_stats[i].user, &cpu_stats[i].nice,
                    &cpu_stats[i].system, &cpu_stats[i].idle, &cpu_stats[i].iowait, 
                    &cpu_stats[i].irq, &cpu_stats[i].softirq, 
                    &garbage, &garbage, &garbage,
                    &bytes_now) != 11)
        {
            return EIO;
        }
        bytes_consumed += bytes_now+1;
        printf("USER: %lu\n", cpu_stats[i].user);
    }
    printf("\n");

    return 0;
}