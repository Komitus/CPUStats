#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "threads_utils.h"

static inline int parse_to_struct(char *buffer, core_stats *cpu_stats, const unsigned short num_of_cores);

void *analyst(void *__thread_struct)
{   
    int *err_num = malloc(sizeof(int));
    *err_num = 0;
    thread_struct *ths = (thread_struct *)__thread_struct;
    char *buffer = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH * ths->num_of_cores);
    core_stats *cpu_stats = (core_stats *)malloc(sizeof *cpu_stats * ths->num_of_cores);
    
    while (1)
    {      
        printf("POPPED\n");
        th_rb_pop_front(ths->th_rb, buffer);
        *err_num = parse_to_struct(buffer, cpu_stats, ths->num_of_cores);
         
        if (*err_num == EIO)
        {
            break;
        }
    }
    return (void *)err_num;
}

/**
 * @brief Parse raw bytes (char*) into struct with proper fileds (cpu stats)
 * @param buffer
 * @param cpu_stats
 * @param num_of_cores
 * @return int - error num
 */
static inline int parse_to_struct(char *buffer, core_stats *cpu_stats, const unsigned short num_of_cores)
{
    int bytes_consumed = 0;
    int bytes_now = 0;

    cpu_stats[0].core_num = -1;
    unsigned int garbage;

    if (sscanf(buffer, "cpu %u %u %u %u %u %u %u %u %u %u%n",
               &cpu_stats[0].user, &cpu_stats[0].nice, &cpu_stats[0].system,
               &cpu_stats[0].idle, &cpu_stats[0].iowait, &cpu_stats[0].irq,
               &cpu_stats[0].softirq,
               &garbage, &garbage, &garbage,
               &bytes_now) != 10)

    {
        return EIO;
    }
    bytes_consumed += bytes_now + 1; // +1 is for  \n

    for (unsigned short i = 1; i < num_of_cores; i++)
    {
        if (sscanf(buffer + bytes_consumed, "cpu%d %u %u %u %u %u %u %u %u %u %u%n",
                   &cpu_stats[i].core_num, &cpu_stats[i].user, &cpu_stats[i].nice,
                   &cpu_stats[i].system, &cpu_stats[i].idle, &cpu_stats[i].iowait,
                   &cpu_stats[i].irq, &cpu_stats[i].softirq,
                   &garbage, &garbage, &garbage,
                   &bytes_now) != 11)
        {
            return EIO;
        }
        bytes_consumed += bytes_now + 1;
        printf("USER: %u\n", cpu_stats[i].user);
    }

    return 0;
}