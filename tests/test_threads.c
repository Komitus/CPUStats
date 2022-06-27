#include "tests_header.h"

void test_threads(void){

    init_shared_data();
    unsigned char thread_num[NUM_OF_THREADS];
    void *(*functions[NUM_OF_THREADS])(void *) = {
    &reader, &analyst, &printer, &watch_dog};

    for(unsigned char i = 0; i < NUM_OF_THREADS; i++){
        thread_num[i] = i;
        atomic_store(&g_shared_data.running[i], 0);
        functions[i](&thread_num[i]);
    }

    assert(g_shared_data.th_rb_for_calculated_data.rb.count == 0);
    assert(g_shared_data.th_rb_for_raw_data.rb.count == 0);

    free_shared_data();
}
