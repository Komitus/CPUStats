#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "threads_utils.h"

static inline void calculate_from_structs(double *cpu_loads, unsigned int *prev_stats,
                                          const CoreStats *cpu_stats, const unsigned short num_of_cores);

void *analyst(void *arg)
{
    MY_LOG("STARTED ANALYST");
    const unsigned char thread_num = *((unsigned char *)arg);
    const unsigned short num_of_cores = g_shared_data.num_of_cores;
    const size_t raw_text_size = sizeof(char) * MAX_LINE_LENGTH * ((unsigned long)num_of_cores + 1);
    char *buffer = (char *)malloc(raw_text_size);
    double *calculated_values = malloc(sizeof(*calculated_values) * (num_of_cores + 1));
    CoreStats *cpu_stats = malloc(sizeof(*cpu_stats) * num_of_cores);
    unsigned int *prev_stats = malloc(sizeof(*prev_stats) * num_of_cores * 2);

    memset(prev_stats, 0, (sizeof(*prev_stats) * num_of_cores * 2));

    while (atomic_load(&g_shared_data.running[thread_num]))
    {
        if (th_rb_pop_front(&g_shared_data.th_rb_for_raw_data, buffer,
                            &g_shared_data.running[thread_num]) != 0)
        {
            break;
        }
        MY_LOG("ANALYST POPPED");
        if (parse_to_struct(buffer, cpu_stats, num_of_cores) == EIO)
        {
            free(buffer);
            break;
        }

        calculate_from_structs(calculated_values, prev_stats, cpu_stats, num_of_cores);
        th_rb_push_back(&g_shared_data.th_rb_for_calculated_data, calculated_values);
        MY_LOG("ANALYST PUSHED");
        atomic_store(&g_shared_data.job_done[thread_num], 1);

    }

    free(buffer);
    free(calculated_values);
    free(cpu_stats);
    free(prev_stats);

    MY_LOG("EXIT ANALYST");

    return NULL;
}

/**
 * @brief Parse raw bytes (char*) into struct with proper fileds (cpu stats)
 * @param buffer raw text data
 * @param cpu_stats struct to which parsed data will be saved
 * @param num_of_cores number of cores (size of cpu_stats array)
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

static inline void calculate_from_structs(double *cpu_loads, unsigned int *prev_stats,
                                          const CoreStats *cpu_stats, const unsigned short num_of_cores)
{
    cpu_loads[num_of_cores] = 0.0;
    for (unsigned short i = 0; i < num_of_cores; i++)
    {
        unsigned int idle = cpu_stats[i].idle + cpu_stats[i].iowait;
        unsigned int non_idle = cpu_stats[i].user + cpu_stats[i].nice +
                                cpu_stats[i].system + cpu_stats[i].irq +
                                cpu_stats[i].softirq + cpu_stats[i].steal;
        unsigned int total = idle + non_idle;

#define WRAP_SUBSTRACT(a, b) (((a) > (b)) ? (a) - (b) : 0)

        unsigned int total_diff = WRAP_SUBSTRACT(total, prev_stats[i * 2]);
        prev_stats[i * 2] = total;

        unsigned int idle_diff = WRAP_SUBSTRACT(idle, prev_stats[i * 2 + 1]);
        prev_stats[i * 2 + 1] = idle;

#undef WRAP_SUBSTRACT

        // that's the case when we read to fast
        if (total_diff == 0)
        {
            cpu_loads[i] = 0.0;
            continue;
        }

        cpu_loads[i] = (10000 * (total_diff - idle_diff) / total_diff) / 100.0;
        cpu_loads[num_of_cores] += cpu_loads[i]; // average load
    }

    cpu_loads[num_of_cores] /= num_of_cores; // average load
}
