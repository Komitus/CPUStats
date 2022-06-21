#ifndef _th_ring_buffer
#define _th_ring_buffer

#include <pthread.h>
#include "ring_buffer.h"

typedef struct th_ring_buffer
{
    ring_buffer *rb;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
} th_ring_buffer;

/* multithread section - is it Open/Close principle? */
void th_rb_init(th_ring_buffer *th_rb, size_t max_entries, size_t data_size);
void th_rb_free(th_ring_buffer *th_rb);
void th_rb_push_back(th_ring_buffer *th_rb, const void *item);
void th_rb_pop_front(th_ring_buffer *th_rb, void *item);

#endif