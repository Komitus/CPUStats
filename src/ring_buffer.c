#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ring_buffer.h"


int rb_init(ring_buffer *rb, size_t max_entries, size_t data_size)
{   

    rb->buffer = malloc(sizeof(char) * max_entries * data_size);
    
    if (rb->buffer == NULL)
    {   
        free(rb);
        return 1;
    }

    rb->buffer_end = (char *)rb->buffer + max_entries * data_size;
    rb->max_entries = max_entries;
    rb->count = 0;
    rb->data_size = data_size;
    rb->head = rb->buffer;
    rb->tail = rb->buffer;

    return 0;
}

void rb_free(ring_buffer *rb)
{
    free(rb->buffer);
}

/*
    [t|h]
    [t|e1][h]
    [t|e1][e2][h]
    [t|e1][e2][e3][h]
    [t|h|e1][e2][e3][e4]   when num of max = 4
    [t|e5][h|e2][e3][e4]
*/
void rb_push_back(ring_buffer *rb, const void *item)
{
    // this method overwrite, i want to have newest elems
    memcpy(rb->head, item, rb->data_size);
    rb->head = (char*)rb->head + rb->data_size;
    if (rb->head == rb->buffer_end)
        rb->head = rb->buffer;

    if (rb->count < rb->max_entries)
    {
        rb->count++;
    }

}

/*
    [t|e1][e2][e3][h]   - popfront
    [t|e2][e3][h]      - tail is in front of head
    [t|e3][h]
    [t|h]
*/
int rb_pop_front(ring_buffer *rb, void *item)
{   

    //printf("Count: %ld\n", rb->count);

    if (rb->count == 0)
    {
        return -1;
    }
    memcpy(item, rb->tail, rb->data_size);
    rb->tail = (char *)rb->tail + rb->data_size;
    if (rb->tail == rb->buffer_end)
        rb->tail = rb->buffer;
    rb->count--;

    return 0;
}
