#ifndef _th_ring_buffer
#define _th_ring_buffer

#include <pthread.h>
#include "ring_buffer.h"

typedef struct ThreadedRingBuffer
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    RingBuffer rb;
} ThreadedRingBuffer;

/* Wrappers to ring_buffer is it Open/Close principle? */

/**
 * @brief Threaded version of rb_init
 * worth to mention that it initalize mutex and cond
 * @param th_rb 
 * @param max_entries 
 * @param data_size 
 */
void th_rb_init(ThreadedRingBuffer *th_rb, size_t max_entries, size_t data_size);
void th_rb_free(ThreadedRingBuffer *th_rb);
void th_rb_push_back(ThreadedRingBuffer *th_rb, const void *item);
void th_rb_pop_front(ThreadedRingBuffer *th_rb, void *item);

#endif