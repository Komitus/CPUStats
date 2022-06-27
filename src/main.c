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
    struct sigaction sa_term;
    memset(&sa_term, 0, sizeof(struct sigaction));
    sa_term.sa_handler = &handle_sig_term;
    sigaction(SIGTERM, &sa_term, NULL);
    sigaction(SIGINT, &sa_term, NULL);
    
    init_shared_data();
    start_all_threads();
    join_all_threads();
    free_shared_data();

    printf("GOOD BYE!\n");

    return 0;
}

