#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <err.h>

#include "header.h"
unsigned short num_of_cores;

// int argc, char *argv[]
int main()
{
    num_of_cores = get_num_of_cores();
    int *err_num = malloc(sizeof *err_num);
    *err_num = 0;

    pthread_t td;
    if (pthread_create(&td, NULL, &reader, NULL) != 0)
        err(EXIT_FAILURE, "Error while creating thread, code: %d\n", errno);

    if (pthread_join(td, (void **)&err_num))
        err(EXIT_FAILURE, "Error while joining thread, code: %d\n", errno);

    if (*err_num != 0)
        err(EXIT_FAILURE, "Error occured, code: %d\n", *err_num);

    return 0;
}
