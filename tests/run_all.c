#include "threads_utils.h"
#include "tests_header.h"


int main(){

    unit_test_ring_buffer_uints();
    printf("\033[0;32mRINGBUFFER UINT TEST PASSED\033[0m\n");
    test_read_file_parse_with_rb();
    printf("\033[0;32mRINGBUFFER ON CHAR* WITH PARSING PASSED\033[0m\n");
    test_threads();
    printf("\033[0;32mTHREADS TESTS PASSED\033[0m\n");

    return 0;
}

