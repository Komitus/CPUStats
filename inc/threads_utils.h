#ifndef _threads_utlis
#define _threads_utlis

#define MAX_LINE_LENGTH  128
#define STATS_FILENAME "/proc/stat"

#include <th_ring_buffer.h>

typedef struct core_stats
{
    int core_num;
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
} core_stats;

typedef struct {
    //Or whatever information that you need
    unsigned int short num_of_cores;
    th_ring_buffer *th_rb;
} thread_struct;


/**
 * @brief Get the num of cores
 * no SOLID (using readfile()) bcs it would add overhead
 * @return unsigned int short - num of cores + 1
 *  (for 4 core cpu its 5) - (+1) is bcs for general stats
 */
unsigned int short get_num_of_cores();
int read_file(char *buffer, const size_t buffer_size, char *filename);

/**
 * @brief  reader function - continously read "/proc/stat"
 * and update varible which analyzer handles
 * @param __thread_struct @see{thread_struct}
 * @return void* - communicate error if occured
 */
void *reader(void *__thread_struct);
void *analyst(void *__thread_struct);


#endif