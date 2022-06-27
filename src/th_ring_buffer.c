#include <stdlib.h>
#include <stdio.h>

#include "th_ring_buffer.h"

void th_rb_init(ThreadedRingBuffer *th_rb, size_t max_entries, size_t data_size)
{
    rb_init(&(th_rb->rb), max_entries, data_size);
    pthread_mutex_init(&th_rb->mutex, NULL);
    pthread_cond_init(&th_rb->cond, NULL);
}

void th_rb_free(ThreadedRingBuffer *th_rb)
{
    rb_free(&(th_rb->rb));
    pthread_mutex_destroy(&th_rb->mutex);
    pthread_cond_destroy(&th_rb->cond);
}

void th_rb_push_back(ThreadedRingBuffer *th_rb, const void *item)
{
    pthread_mutex_lock(&th_rb->mutex);
    rb_push_back(&th_rb->rb, item);
    pthread_mutex_unlock(&th_rb->mutex);
    pthread_cond_signal(&th_rb->cond);
}

int th_rb_pop_front(ThreadedRingBuffer *th_rb, void *item, atomic_bool *running)
{
    pthread_mutex_lock(&th_rb->mutex);
    while (th_rb->rb.count == 0)
    {   
        if(atomic_load(running) == 0){
            pthread_mutex_unlock(&th_rb->mutex);
            return 1;
        }
        // it waits here so i dont ready queue size endlessly
        pthread_cond_wait(&th_rb->cond, &th_rb->mutex);
    }
    rb_pop_front(&th_rb->rb, item);
    pthread_mutex_unlock(&th_rb->mutex);
    return 0;
}

void th_rb_interrupt_signal(ThreadedRingBuffer *th_rb){
    pthread_cond_signal(&th_rb->cond);
}
