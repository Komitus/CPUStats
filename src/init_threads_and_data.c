#include <stdlib.h>
#include <err.h>
#include <assert.h>

#include "threads_utils.h"

SharedData g_shared_data;
const char *THREAD_STRING[] = {
    "reader", "analyst", "printer", "watchdog",
#ifdef LOGGER_ON
    "logger"
#endif
};

static void *(*functions[NUM_OF_THREADS])(void *) = {
    &reader, &analyst, &printer, &watch_dog,
#ifdef LOGGER_ON
    &logger
#endif
};

static pthread_t thread_init_threads[NUM_OF_THREADS];
static unsigned char threads_num[NUM_OF_THREADS];

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

    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        atomic_store(&g_shared_data.running[i], 1);
        if (i < NUM_OF_OBLIGATORY_THREADS)
        {
            atomic_store(&g_shared_data.job_done[i], 0);
        }
    }

    size_t raw_text_size = sizeof(char) * MAX_LINE_LENGTH * ((unsigned long)num_of_cores + 1);
    size_t calculated_values_size = sizeof(double) * ((unsigned long)num_of_cores + 1);

    th_rb_init(&g_shared_data.th_rb_for_raw_data, MAX_BUFF_ENTRIES, raw_text_size);
    th_rb_init(&g_shared_data.th_rb_for_calculated_data, MAX_BUFF_ENTRIES, calculated_values_size);

#ifdef LOGGER_ON
    th_rb_init(&g_shared_data.th_rb_for_logger, 30, sizeof(char) * MAX_LINE_LENGTH);
#endif
}

void free_shared_data(void)
{
    th_rb_free(&g_shared_data.th_rb_for_raw_data);
    th_rb_free(&g_shared_data.th_rb_for_calculated_data);
#ifdef LOGGER_ON
    th_rb_free(&g_shared_data.th_rb_for_logger);
#endif
}

void start_all_threads(void)
{
    for (unsigned char i = 0; i < NUM_OF_THREADS; i++)
    {
        threads_num[i] = i;
        if (pthread_create(&thread_init_threads[i], NULL, functions[i], &threads_num[i]) != 0)
        {
            err(EXIT_FAILURE, "Error while creating thread %s\n", THREAD_STRING[i]);
        }
    }
}

void join_all_threads(void)
{
    for (unsigned char i = 0; i < NUM_OF_OBLIGATORY_THREADS; i++)
    {
        // printf("Thread id %lu\n", threads_num[i]);
        if (pthread_join(thread_init_threads[i], NULL))
        {
            fprintf(stderr, "Error while joining thread %s\n", THREAD_STRING[i]);
        }
    }
    if (pthread_join(thread_init_threads[WATCHDOG_TH_NUM], NULL))
    {
        fprintf(stderr, "Error while joining thread %s\n", THREAD_STRING[WATCHDOG_TH_NUM]);
    }

#ifdef LOGGER_ON
    atomic_store(&g_shared_data.running[LOGGER_TH_NUM], 0);
    th_rb_interrupt_signal(&g_shared_data.th_rb_for_logger);
    if (pthread_join(thread_init_threads[LOGGER_TH_NUM], NULL))
    {
        fprintf(stderr, "Error while joining thread %s\n", THREAD_STRING[LOGGER_TH_NUM]);
    }
#endif
}

void stop_all_threads(void)
{
    atomic_store(&g_shared_data.running[WATCHDOG_TH_NUM], 0);

    for (int j = 0; j < NUM_OF_OBLIGATORY_THREADS; j++)
    {
        atomic_store(&g_shared_data.running[j], 0);
    }
    th_rb_interrupt_signal(&g_shared_data.th_rb_for_raw_data);
    th_rb_interrupt_signal(&g_shared_data.th_rb_for_calculated_data);
}

void handle_sig_term(int signum)
{
    UNUSED(signum);
    fprintf(stderr, "\nEXITING, PLEASE WAIT!!!\n");
    stop_all_threads();
}
