#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "threads_utils.h"

static inline void calculate_from_structs(double *cpu_loads, const CoreStats *prev_stats,
                                          const CoreStats *curr_stats, const unsigned short num_of_cores);

void *analyst(void *arg)
{
    ThreadStruct *thread_struct = (ThreadStruct *)arg;
    const unsigned short num_of_cores = thread_struct->num_of_cores;
    ThreadedRingBuffer *th_rb_for_receiving = thread_struct->th_rbs[0];
    ThreadedRingBuffer *th_rb_for_sending = thread_struct->th_rbs[1];

    char *buffer = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH * (num_of_cores + 1));
    double to_send[num_of_cores + 1];   // use VLA bcs its small size (which I am sure about)
    CoreStats prev_stats[num_of_cores]; // don't do stack overlfow
    CoreStats curr_stats[num_of_cores];

    th_rb_pop_front(th_rb_for_receiving, buffer);
    if (parse_to_struct(buffer, prev_stats, num_of_cores) == EIO)
    {
        free(buffer);
        return NULL;
    }

    while (1)
    {
        sleep(1);
        th_rb_pop_front(th_rb_for_receiving, buffer);

        if (parse_to_struct(buffer, curr_stats, num_of_cores) == EIO)
        {
            free(buffer);
            break;
        }
        calculate_from_structs(to_send, prev_stats, curr_stats, num_of_cores);

        th_rb_push_back(th_rb_for_sending, to_send);

        memcpy(prev_stats, curr_stats, sizeof(CoreStats) * num_of_cores);
    }

    return NULL;
}

/**
 * @brief Parse raw bytes (char*) into struct with proper fileds (cpu stats)
 * @param buffer
 * @param cpu_stats
 * @param num_of_cores
 * @return int - error num
 */
int parse_to_struct(char *buffer, CoreStats *cpu_stats, const unsigned short num_of_cores)
{
    int bytes_consumed = 0;
    int bytes_now = 0;

    unsigned int garbage;

    if (sscanf(buffer, "cpu %u %u %u %u %u %u %u %u %u %u%n",
               &garbage, &garbage, &garbage, &garbage,
               &garbage, &garbage, &garbage, &garbage,
               &garbage, &garbage,
               &bytes_now) != 10)

    {
        return EIO;
    }
    bytes_consumed += bytes_now + 1; // +1 is for  \n

    for (unsigned short i = 0; i < num_of_cores; i++)
    {
        if (sscanf(buffer + bytes_consumed, "cpu%u %u %u %u %u %u %u %u %u %u %u%n",
                   &garbage, &cpu_stats[i].user, &cpu_stats[i].nice,
                   &cpu_stats[i].system, &cpu_stats[i].idle, &cpu_stats[i].iowait,
                   &cpu_stats[i].irq, &cpu_stats[i].softirq, &cpu_stats[i].steal,
                   &garbage, &garbage,
                   &bytes_now) != 11)
        {
            return EIO;
        }
        bytes_consumed += bytes_now + 1;
    }

    return 0;
}

static inline void calculate_from_structs(double *cpu_loads, const CoreStats *prev_stats,
                                          const CoreStats *curr_stats, const unsigned short num_of_cores)
{

    for (unsigned short i = 0; i < num_of_cores; i++)
    {
        unsigned int prev_idle = prev_stats[i].idle + prev_stats[i].iowait;
        unsigned int curr_idle = curr_stats[i].idle + curr_stats[i].iowait;

        unsigned int prev_non_idle = prev_stats[i].user + prev_stats[i].nice +
                                     prev_stats[i].system + prev_stats[i].irq + prev_stats[i].softirq + prev_stats[i].steal;
        unsigned int curr_non_idle = curr_stats[i].user + curr_stats[i].nice +
                                     curr_stats[i].system + curr_stats[i].irq + curr_stats[i].softirq + curr_stats[i].steal;

        unsigned int prev_total = prev_idle + prev_non_idle;
        unsigned int curr_total = curr_idle + curr_non_idle;

        unsigned int res_total = curr_total - prev_total;
        unsigned int res_idle = curr_idle - prev_idle;

        cpu_loads[i] = (10000 * (res_total - res_idle) / res_total + 1) / 100.0;

        cpu_loads[num_of_cores] += cpu_loads[i]; // average load
    }

    cpu_loads[num_of_cores] /= num_of_cores; // average load
}
