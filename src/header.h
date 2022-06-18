#ifndef _header
#define _header

#define NUM_OF_REQ_FIELDS 8
#define MAX_LINE_LENGTH 1024
#define STATS_FILENAME "/proc/stat"

typedef struct CoreStats
{
    char core_num[255];
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
} CoreStats;

extern unsigned short num_of_cores; // i could copy it for every thread but what for?
                                    // i assing this once, then no modifications

/**
 * @brief Get the num of cores
 *
 * @return unsigned short - num of cores + 1
 *  (for 4 core cpu its 5) - (+1) is bcs for general stats
 */
unsigned short get_num_of_cores();

/**
 * @brief reader function - count number of cpus and then continously
 * read "/proc/stat" and update varible which analyzer handles
 * @return void* - communicate error if occured
 */
void *reader();
// void printCpuStats(CoreStats *CoreStats);

#endif