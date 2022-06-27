#ifndef _th_ring_buffer
#define _th_ring_buffer

#include <pthread.h>
#include "ring_buffer.h"
#include <stdatomic.h>

typedef struct ThreadedRingBuffer
{
    RingBuffer rb;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

} ThreadedRingBuffer;

/* Wrappers to ring_buffer is it Open/Close principle? */

/**
 * @brief Threaded version of rb_init
 * worth to mention that it initalize mutex and cond
 * @param th_rb ptr to ThreadedRingBuffer
 * @param max_entries max numb of item to fit in ringbuffer
 * @param data_size size of single item
 */
void th_rb_init(ThreadedRingBuffer *th_rb, size_t max_entries, size_t data_size);
void th_rb_free(ThreadedRingBuffer *th_rb);
void th_rb_push_back(ThreadedRingBuffer *th_rb, const void *item);
int th_rb_pop_front(ThreadedRingBuffer *th_rb, void *item, atomic_bool *running);
/**
 * @brief this function is necessary when program is exiting and th_rb_pop_front 
 * is freezed on cond_wait, it just sends signal to cond member of th_rb
 * 
 * @param th_rb ptr to ThreadedRingBuffer
 */
void th_rb_interrupt_signal(ThreadedRingBuffer *th_rb);

#endif
