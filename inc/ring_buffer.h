#ifndef _ring_buffer
#define _ring_buffer

#include <stddef.h>
#include <pthread.h>

/**
 * @brief RingBuffer module
 */
typedef struct RingBuffer
{
    void *buffer;
    void *buffer_end; 
    size_t max_entries;
    size_t count;
    size_t data_size;
    void *head;
    void *tail;
} RingBuffer;

int rb_init(RingBuffer *rb, size_t max_entries, size_t data_size);
void rb_free(RingBuffer *rb);
/**
 * @brief Pushes item to ring buffer,
 * overwrite the oldest item
 * @param rb - ring buffer
 * @param item  - item to push, its copied
 */
void rb_push_back(RingBuffer *rb, const void *item);
/**
 * @brief Pop item from ring buffer
 * @details Valgrind is complaining about memory, bcs of using pointer,
 * important is to take care of freeing rb.buffer (rb_free func)
 * @param rb pointer to ringbuffer
 * @param item pointer to the element to which the oldest item 
 * is to be copied
 * @return int status - return -1 if empty
 */
int rb_pop_front(RingBuffer *rb, void *item);

int test_ring_buffer_strings(void);
int test_ring_buffer_uints(void);

#endif
