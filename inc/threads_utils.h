#ifndef _threads_utlis
#define _threads_utlis

#define MAX_LINE_LENGTH  128
#define STATS_FILENAME "/proc/stat"

#include <th_ring_buffer.h>

typedef struct CoreStats
{
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
    unsigned int steal;
} CoreStats;

typedef struct ThreadStruct{
    //Or whatever information that you need
    unsigned int short num_of_cores;
    ThreadedRingBuffer **th_rbs;

} ThreadStruct;


/**
 * @brief Get the num of cores
 * no SOLID (using readfile()) bcs it would add overhead
 * @return unsigned int short - num of cores + 1
 *  (for 4 core cpu its 5) - (+1) is bcs for general stats
 */
int get_num_of_cores();

/**
 * @brief  reader function - continously read "/proc/stat"
 * and update varible which analyzer handles
 * @param __thread_struct @see{ThreadStruct}
 * @return void* - communicate error if occured
 */
void *reader(void *arg);
int read_file(char *buffer, const size_t buffer_size, char *filename);

void *analyst(void *arg);
int parse_to_struct(char *buffer, CoreStats *cpu_stats, const unsigned short num_of_cores);

void *printer(void *arg);

#endif