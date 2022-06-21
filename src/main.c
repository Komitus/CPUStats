#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <err.h>

#include "threads_utils.h"

unsigned short num_of_cores;
#define NUM_OF_THREADS 2
// int argc, char *argv[]
int main()
{   
    unsigned short num_of_cores = get_num_of_cores();

    thread_struct thread_structs[NUM_OF_THREADS];
    th_ring_buffer *th_rb = malloc(sizeof(th_ring_buffer));
    ring_buffer *rb =  malloc(sizeof(ring_buffer));
    th_rb->rb = rb;
    th_rb_init(th_rb, 15, sizeof(char) * MAX_LINE_LENGTH * num_of_cores);

    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        thread_structs[i].num_of_cores = num_of_cores;
        thread_structs[i].th_rb = th_rb;
    }

    int *err_num = malloc(sizeof *err_num);
    *err_num = 0;

    pthread_t reader_th;
    //pthread_t analyst_th;
    
    if (pthread_create(&reader_th, NULL, &reader, &thread_structs[0]) != 0)
    {
        fprintf(stderr, "Error while creating thread, code: %d\n", errno);
    }

    /*
    if (pthread_create(&analyst_th, NULL, &analyst, &thread_structs[1]) != 0)
    {
        fprintf(stderr, "Error while creating thread, code: %d\n", errno);
    }*/
    
    if (pthread_join(reader_th, (void **)&err_num))
    {
        fprintf(stderr, "Error while joining thread, code: %d\n", errno);
    }
    /*
    if (pthread_join(analyst_th, (void **)&err_num))
    {
        fprintf(stderr, "Error while joining thread, code: %d\n", errno);
    }
    */

    if (*err_num != 0)
    {
        fprintf(stderr, "Error occured, code: %d\n", *err_num);
    }

    free(err_num);

    return 0;
}
