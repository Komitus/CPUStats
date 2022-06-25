#define  _POSIX_C_SOURCE  200809L
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "threads_utils.h"

int main()
{   
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &handle_sig_term;
    sigaction(SIGTERM, &sa, NULL);
    
    init_shared_data();
    start_all_threads();
    join_all_threads();
    free_shared_data();

    return 0;
}

