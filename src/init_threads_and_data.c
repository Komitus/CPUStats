#include <stdlib.h>
#include <err.h>
#include <assert.h>

#include "threads_utils.h"

/* globals */
SharedData g_shared_data;
static pthread_t thread_init_threads[NUM_OF_THREADS];
static pthread_t threads_ids[NUM_OF_THREADS];

const char *THREAD_STRING[] = {
    "reader", "analyst", "printer", "watchdog"};
static void *(*functions[NUM_OF_THREADS])(void *) = {
    &reader, &analyst, &printer, &watch_dog};

/* end of globals */

void init_shared_data(void)
{

    int num_of_cores = get_num_of_cores();

    if (num_of_cores <= 0 || num_of_cores > MAX_NUM_OF_CORES)
    {
        err(EXIT_FAILURE, "Cannot get proper number of cores\n");
    }
    assert(num_of_cores > 0 || num_of_cores < MAX_NUM_OF_CORES);

    printf("Number of cores: %d\n", num_of_cores);
    g_shared_data.num_of_cores = (unsigned short)num_of_cores;

    for (unsigned char i = 0; i < NUM_OF_THREADS; i++)
    {   
        atomic_store(&g_shared_data.running[i], 1);
        atomic_store(&g_shared_data.job_done[i], 0);
    }

    size_t raw_text_size = sizeof(char) * MAX_LINE_LENGTH * ((unsigned long)num_of_cores + 1);
    size_t calculated_values_size = sizeof(double) * ((unsigned long)num_of_cores + 1);

    th_rb_init(&g_shared_data.th_rb_for_raw_data, MAX_BUFF_ENTRIES, raw_text_size);
    th_rb_init(&g_shared_data.th_rb_for_calculated_data, MAX_BUFF_ENTRIES, calculated_values_size);
}

void free_shared_data(void)
{
    th_rb_free(&g_shared_data.th_rb_for_raw_data);
    th_rb_free(&g_shared_data.th_rb_for_calculated_data);
}

void start_all_threads(void)
{
    for (unsigned char i = 0; i < NUM_OF_THREADS; i++)
    {
        threads_ids[i] = i;
        if (pthread_create(&thread_init_threads[i], NULL, functions[i], &threads_ids[i]) != 0)
        {
            fprintf(stderr, "Error while creating thread %s\n", THREAD_STRING[i]);
        }
    }
}

void join_all_threads(void)
{
    for (unsigned char i = 0; i < NUM_OF_THREADS; i++)
    {
        // printf("Thread id %lu\n", threads_ids[i]);
        if (pthread_join(thread_init_threads[i], NULL))
        {
            fprintf(stderr, "Error while joining thread %s\n", THREAD_STRING[i]);
        }
    }
}

void stop_all_threads(void)
{
    for (int j = 0; j < NUM_OF_THREADS; j++)
    {
        atomic_store(&g_shared_data.running[j], 0);
    }
    th_rb_interrupt_signal(&g_shared_data.th_rb_for_raw_data);
    th_rb_interrupt_signal(&g_shared_data.th_rb_for_calculated_data);
}

void handle_sig_term(int signum)
{
    fprintf(stderr, "Exited by signal nr: %d\n", signum);
    stop_all_threads();
}

