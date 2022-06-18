#ifndef _header
#define _header

#define NUM_OF_REQ_FIELDS 8
#define MAX_LINE_LENGTH 1024
#define STATS_FILENAME "/proc/stat"
#define NUM_OF_THREADS 4

typedef struct CoreStats
{
    int core_num;
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
} CoreStats;

/**
 * @brief Get the num of cores
 * no SOLID (using readfile()) bcs it would add overhead
 * @return unsigned short - num of cores + 1
 *  (for 4 core cpu its 5) - (+1) is bcs for general stats
 */
unsigned short get_num_of_cores();

/**
 * @brief reader function - continously read "/proc/stat"
 * and update varible which analyzer handles
 * @return void* - communicate error if occured
 */
void *reader();



#endif