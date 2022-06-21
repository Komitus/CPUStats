#ifndef _ring_buffer
#define _ring_buffer

#include <stddef.h>
#include <pthread.h>

/**
 * @brief ring_buffer module
 */
typedef struct ring_buffer
{
    void *buffer;
    void *buffer_end; 
    size_t max_entries;
    size_t count;
    size_t data_size;
    void *head;
    void *tail;
} ring_buffer;

int rb_init(ring_buffer *rb, size_t max_entries, size_t data_size);
void rb_free(ring_buffer *rb);
void rb_push_back(ring_buffer *rb, const void *item);
int rb_pop_front(ring_buffer *rb, void *item);

int test_ring_buffer_strings();
int test_ring_buffer_uints();

#endif