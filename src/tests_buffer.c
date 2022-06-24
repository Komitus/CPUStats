#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "threads_utils.h"


int test_ring_buffer_uints(){

    const unsigned int num_of_pushes = 20000;
    const unsigned int max_entries = 1000;
    const unsigned int num_of_pops = max_entries;

    RingBuffer rb;
 
    rb_init(&rb, max_entries, sizeof(unsigned int));

    unsigned int i, val;

    for(i = 0; i< num_of_pushes; i++){
        //printf("Pushed:%u\n", i);
        rb_push_back(&rb, &i);
    }
    printf("Pushed: %u times\n", i);
    assert(i = num_of_pushes);

    for(i =0; i< num_of_pops; i++){
        rb_pop_front(&rb, &val);
        //printf("Popped: %u\n", *val);
        assert(num_of_pushes - max_entries + i == val);
    }
    printf("Popped: %u times\n", i);
    assert(i = num_of_pops);

    rb_free(&rb);

    return 0;
}

int test_ring_buffer_strings(){

    const unsigned int num_of_pushes = 1500;
    const unsigned int num_of_pops = 400;
    const unsigned int max_entries = 1000;

    unsigned short num_of_cores = get_num_of_cores(); 

    RingBuffer rb;
    const size_t buff_size = sizeof(char) * MAX_LINE_LENGTH * (num_of_cores+1);
    rb_init(&rb, max_entries, buff_size);

    char *buffer = malloc(buff_size);
    read_file(buffer, buff_size, STATS_FILENAME);

    unsigned int i;
    for(i=0; i < num_of_pushes; i++){
        //printf("Pushed:\n%s\n", buffer);
        rb_push_back(&rb, buffer);
        
    }
    printf("Pushed %d times\n", i);
    assert(i == num_of_pushes);

    CoreStats cpu_stats[num_of_cores];

    for(i = 0; i< num_of_pops; i++){
        rb_pop_front(&rb, buffer);

        buffer[buff_size-1] = '\0';
        size_t j;
        for(j = 0; j < strlen(buffer); j++){
            //printf("%c",buffer[j]);
        }
        //printf("Real buff_size: %lu\n", strlen(buffer));
        //printf("Counter: %ld\tBuff_size: %ld\n", j, buff_size);

        assert(j ==  strlen(buffer)); 
        assert(parse_to_struct(buffer, cpu_stats, num_of_cores) == 0);          
    }

    assert(i == num_of_pops);
    printf("\nPopped %d times\n", i);

    rb_free(&rb);
    free(buffer);

    return 0;
}