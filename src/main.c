#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <err.h>

#include "threads_utils.h"

unsigned short num_of_cores;

#define MAX_BUFF_ENTRIES 15

#define NUM_OF_THREADS 3
#define READER_TH_NUM 0
#define ANALYST_TH_NUM 1
#define PRINTER_TH_NUM 2

// int argc, char *argv[]
int main()
{
    int num_of_cores = get_num_of_cores();

    if (num_of_cores <= 0)
    {
        err(EXIT_FAILURE, "Cannot get number of cores\n");
    }

    printf("Number of cores: %d\n", num_of_cores);

    // ThreadStruct thread_structs[NUM_OF_THREADS];
    ThreadStruct thread_structs[NUM_OF_THREADS];
    ThreadedRingBuffer th_rb_for_reading;
    ThreadedRingBuffer th_rb_for_printing;

    th_rb_init(&th_rb_for_reading, MAX_BUFF_ENTRIES, sizeof(char) * MAX_LINE_LENGTH * (num_of_cores + 1));
    th_rb_init(&th_rb_for_printing, MAX_BUFF_ENTRIES, sizeof(double) * (num_of_cores + 1));

    for (unsigned short i = 0; i < NUM_OF_THREADS; i++)
    {
        thread_structs[i].num_of_cores = num_of_cores;
    }

    thread_structs[READER_TH_NUM].th_rbs = malloc(sizeof(ThreadedRingBuffer *));
    thread_structs[READER_TH_NUM].th_rbs[0] = &th_rb_for_reading;

    thread_structs[ANALYST_TH_NUM].th_rbs = malloc(sizeof(ThreadedRingBuffer *) * 2);
    thread_structs[ANALYST_TH_NUM].th_rbs[0] = &th_rb_for_reading;
    thread_structs[ANALYST_TH_NUM].th_rbs[1] = &th_rb_for_printing;

    thread_structs[PRINTER_TH_NUM].th_rbs = malloc(sizeof(ThreadedRingBuffer *));
    thread_structs[PRINTER_TH_NUM].th_rbs[0] = &th_rb_for_printing;

    pthread_t reader_th, analyst_th, printer_th;

    if (pthread_create(&reader_th, NULL, &reader, &thread_structs[READER_TH_NUM]) != 0)
    {
        fprintf(stderr, "Error while creating thread \n");
    }

    if (pthread_create(&analyst_th, NULL, &analyst, &thread_structs[ANALYST_TH_NUM]) != 0)
    {
        fprintf(stderr, "Error while creating thread\n");
    }

    if (pthread_create(&printer_th, NULL, &printer, &thread_structs[PRINTER_TH_NUM]) != 0)
    {
        fprintf(stderr, "Error while creating thread\n");
    }

    pthread_exit(NULL);
    return 0;
}
