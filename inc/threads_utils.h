#ifndef _threads_utlis
#define _threads_utlis

#include <th_ring_buffer.h>
#include <limits.h>
#include <stdatomic.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

//#define LOGGER_ON

#define UNUSED(x) (void)(x)
#define MAX_LINE_LENGTH 128
#define PRINT_LINE_LENGTH 20
#define STATS_FILENAME "/proc/stat"
#define MAX_NUM_OF_CORES USHRT_MAX
#define MAX_BUFF_ENTRIES 5

#define NUM_OF_OBLIGATORY_THREADS 3
#define WATCHDOG_TH_NUM 3
#define LOGGER_TH_NUM 4

#ifdef LOGGER_ON
#define NUM_OF_THREADS 5
#define LOG_FILE "logs.txt"
#define MY_LOG(msg) th_rb_push_back(&g_shared_data.th_rb_for_logger, msg)
#else
#define NUM_OF_THREADS 4
#define MY_LOG(msg) UNUSED(msg)
#endif

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

/**
 * @brief Global data
 */
typedef struct SharedData
{
    ThreadedRingBuffer th_rb_for_raw_data;
    ThreadedRingBuffer th_rb_for_calculated_data;
#ifdef LOGGER_ON
    ThreadedRingBuffer th_rb_for_logger;
#endif
    atomic_bool running[NUM_OF_THREADS];
    atomic_bool job_done[NUM_OF_OBLIGATORY_THREADS];
    unsigned short num_of_cores;

} SharedData;

/**

 */
int get_num_of_cores(void);

/**
 * @brief continously read "/proc/stat"
 *        update varible which analyzer handles
 *
 * @param arg - thread num (from for loop)
 * @return void* - NULL
 */
void *reader(void *arg);
int read_file(char *buffer, const size_t buffer_size, char *filename);
/**
 * @brief analyses data (raw plaintext) which reader function
 *        pushed to theathered ring buffer for reading,
 *        and sends calculated datata to
 *        threated ring buffer for printing
 *
 * @param arg - thread num (from for loop)
 * @return void* - NULL
 */
void *analyst(void *arg);
/**
 * @brief analyst use it for parsing plaintext into CoreStats
 *
 * @param buffer - raw plaintext
 * @param cpu_stats - destination of parsing
 * @param num_of_cores - it is what it is
 * @return int - error code - 0 if SUCCESS
 */
int parse_to_struct(char *buffer, CoreStats *cpu_stats, const unsigned short num_of_cores);

/**
 * @brief print cpu usage in percentage
 *
 * @param arg - thread num (from for loop)
 * @return void*
 */
void *printer(void *arg);

/**
 * @brief makes sure the program doesn't crash,
 *        checks the job_done variable every two seconds,
 *        if it's still set to false (0)
 *        it exits giving a signal to other threads to finish
 *
 * @param arg - thread num (from for loop)
 * @return void*
 */
void *watch_dog(void *arg);

extern SharedData g_shared_data;
extern const char *THREAD_STRING[];

#ifdef LOGGER_ON
/**
 * @brief It's saving messges from threads to file
 * 
 * @param arg 
 * @return void* 
 */
void *logger(void *arg);
#endif

/* init_threads_and_data.c */
/**
 * @brief this function initialise struct SharedData (g_shared_data)
 */
void init_shared_data(void);
void free_shared_data(void);
void start_all_threads(void);
void join_all_threads(void);
void stop_all_threads(void);
void handle_sig_term(int signum);

#endif
