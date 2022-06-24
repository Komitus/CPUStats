#ifndef _threads_utlis
#define _threads_utlis

#include <th_ring_buffer.h>
#include <limits.h>

#define MAX_LINE_LENGTH 128
#define NOT_READ_CHAR 1
#define STATS_FILENAME "/proc/stat"
#define MAX_NUM_OF_CORES USHRT_MAX
#define MAX_BUFF_ENTRIES 10

#define NUM_OF_THREADS 3
#define READER_TH_NUM 0
#define ANALYST_TH_NUM 1
#define PRINTER_TH_NUM 2
#define PRINT_LINE_LENGTH 20

typedef struct CoreStats
{
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
} CoreStats;

typedef struct ThreadStruct
{
    // Or whatever information that you need
    unsigned short num_of_cores;
    ThreadedRingBuffer **th_rbs;

} ThreadStruct;

/**
 * @brief Get the num of cores
 * no SOLID (using readfile()) bcs it would add overhead
 * @return  unsigned long long  short - num of cores
 *  (for 4 core cpu its 4)
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