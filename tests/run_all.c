#include "threads_utils.h"
#include "tests_header.h"


int main(){

    unit_test_ring_buffer_uints();
    test_read_file_parse_with_rb();
    test_threads();

    return 0;
}

