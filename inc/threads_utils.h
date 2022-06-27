#ifndef _threads_utlis
#define _threads_utlis

#include <th_ring_buffer.h>
#include <limits.h>
#include <stdatomic.h>
#include <stdio.h>
#include <assert.h>

#define UNUSED(x) (void)(x)
#define MAX_LINE_LENGTH 128
#define STATS_FILENAME "/proc/stat"
#define MAX_NUM_OF_CORES USHRT_MAX
#define MAX_BUFF_ENTRIES 5

#define NUM_OF_OBLIGATORY_THREADS 3
#define NUM_OF_THREADS 4
#define PRINT_LINE_LENGTH 20

typedef struct CoreStats
{
    unsigned int user;
    unsigned int  nice;
    unsigned int  system;
    unsigned int  idle;
    unsigned int  iowait;
    unsigned int  irq;
    unsigned int  softirq;
    unsigned int  steal;
} CoreStats;

typedef struct SharedData
{
    ThreadedRingBuffer th_rb_for_raw_data;
    ThreadedRingBuffer th_rb_for_calculated_data;
    atomic_bool running[NUM_OF_THREADS];
    time_t last_time_active;
    pthread_cond_t time_cond;
    pthread_mutex_t time_mutex;
    unsigned short num_of_cores;
    
} SharedData;

/**
 * @brief Get the num of cores
 * no SOLID (using readfile()) bcs it would add overhead
 * @return  unsigned int   short - num of cores
 *  (for 4 core cpu its 4)
 */
int get_num_of_cores(void);

/**
 * @brief  reader function - continously read "/proc/stat"
 * and update varible which analyzer handles
 * @return void* - communicate error if occured
 */
void *reader(void *arg);
int read_file(char *buffer, const size_t buffer_size, char *filename);

void *analyst(void *arg);
int parse_to_struct(char *buffer, CoreStats *cpu_stats, const unsigned short num_of_cores);

void *printer(void *arg);
void *watch_dog(void *arg);
void *logger(void *arg);

extern SharedData g_shared_data;
extern const char *THREAD_STRING[];


/* init_threads_and_data.c */
void init_shared_data(void);
void free_shared_data(void);
void start_all_threads(void);
void join_all_threads(void);
void stop_all_threads(void);
void handle_sig_term(int signum);
void handle_sig_int(int signum);


#endif
