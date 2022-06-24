#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "threads_utils.h"

static inline void calculate_from_structs(double *cpu_loads, unsigned long long (*prev_stats)[2],
                                          const CoreStats *cpu_stats, const unsigned short num_of_cores);

void *analyst(void *arg)
{   
    ThreadStruct *thread_struct = (ThreadStruct *)arg;
    const unsigned short num_of_cores = thread_struct->num_of_cores;
    assert (num_of_cores > 0 || num_of_cores < MAX_NUM_OF_CORES);
    ThreadedRingBuffer *th_rb_for_receiving = thread_struct->th_rbs[0];
    ThreadedRingBuffer *th_rb_for_sending = thread_struct->th_rbs[1];

    char *buffer = (char *)malloc(sizeof(char) * MAX_LINE_LENGTH * (num_of_cores + 1));
    double to_send[num_of_cores + 1];  // use VLA bcs its small size (which I am sure about)
    CoreStats cpu_stats[num_of_cores]; // don't do stack overlfow
    unsigned long long prev_stats[num_of_cores][2];
    memset(prev_stats, 0, (sizeof(unsigned long long) * num_of_cores * 2));

    while (1)
    {
        th_rb_pop_front(th_rb_for_receiving, buffer);

        if (parse_to_struct(buffer, cpu_stats, num_of_cores) == EIO)
        {
            free(buffer);
            break;
        }

        calculate_from_structs(to_send, prev_stats, cpu_stats, num_of_cores);

        th_rb_push_back(th_rb_for_sending, to_send);
    }

    th_rb_free(th_rb_for_sending);
    free(buffer);

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

    unsigned long long garbage;

    if (sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu%n",
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
        if (sscanf(buffer + bytes_consumed, "cpu%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu%n",
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

static inline void calculate_from_structs(double *cpu_loads, unsigned long long (*prev_stats)[2],
                                          const CoreStats *cpu_stats, const unsigned short num_of_cores)
{
    cpu_loads[num_of_cores] = 0.0;
    for (unsigned short i = 0; i < num_of_cores; i++)
    {
        unsigned long long idle = cpu_stats[i].idle + cpu_stats[i].iowait;
        unsigned long long non_idle = cpu_stats[i].user + cpu_stats[i].nice +
                                      cpu_stats[i].system + cpu_stats[i].irq + cpu_stats[i].softirq + cpu_stats[i].steal;
        unsigned long long total = idle + non_idle;

#define WRAP_SUBSTRACT(a, b) (((a) > (b)) ? (a) - (b) : 0)

        unsigned long long total_diff = WRAP_SUBSTRACT(total, prev_stats[i][0]);
        prev_stats[i][0] = total;

        unsigned long long idle_diff = WRAP_SUBSTRACT(idle, prev_stats[i][1]);
        prev_stats[i][1] = idle;

#undef WRAP_SUBSTRACT

        //that's the case when we read to fast
        if (total_diff == 0)
        {
            cpu_loads[i] = 0.0;
            continue;
        }

        cpu_loads[i] = (10000 * (total_diff - idle_diff) / total_diff + 1) / 100.0;
        cpu_loads[num_of_cores] += cpu_loads[i]; // average load
    }

    cpu_loads[num_of_cores] /= num_of_cores; // average load
}