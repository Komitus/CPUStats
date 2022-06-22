#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

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

    return 0;
}

int test_ring_buffer_strings(){

    const unsigned int num_of_pushes = 1500;
    const unsigned int num_of_pops = 400;
    const unsigned int max_entries = 1000;

    RingBuffer rb;
    const size_t buff_size = sizeof(char) * MAX_LINE_LENGTH * 5;
    rb_init(&rb, max_entries, buff_size);

    char *buffer = malloc(buff_size);
    read_file(buffer, buff_size, STATS_FILENAME);

    unsigned int i;
    for(i=0; i< num_of_pushes; i++){
        //printf("Pushed:\n%s\n", buffer);
        rb_push_back(&rb, buffer);
    }

    printf("Pushed %d times\n", i);
    assert(i == num_of_pushes);

    unsigned short num_of_cores = get_num_of_cores(); 
    CoreStats cpu_stats[num_of_cores];

    for(i = 0; i< num_of_pops; i++){
        rb_pop_front(&rb, buffer);

         size_t j;

        for(j = 0; buffer[j] != '\0'; j++){
            //printf("%c",buffer[j]);
        }
        //printf("Counter: %ld\tBuff_size: %ld\n", j, buff_size);

        assert(j == buff_size);
        assert(parse_to_struct(buffer, cpu_stats, num_of_cores) == 0);          
    }

    assert(i == num_of_pops);
    printf("Popped %d times\n", i);

    return 0;
}