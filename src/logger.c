#include "threads_utils.h"
#include <stdlib.h>
#include <string.h>

#ifdef LOGGER_ON
#define MAX_DATE_SIZE 30

static void get_time(char *date_str){
    struct tm *sTm;
    time_t now;
    now = time(0);
    sTm = gmtime(&now);
    strftime(date_str, MAX_DATE_SIZE, "%Y-%m-%d %H:%M:%S", sTm);
}

void *logger(void *arg)
{
    MY_LOG("LOGGER STARTED");
    const unsigned char thread_num = *((unsigned char *)arg);
    const size_t msg_size = sizeof(char) * MAX_LINE_LENGTH;
    char *date_str = malloc(MAX_DATE_SIZE);
    char *msg = malloc(msg_size);
    FILE *out_file = fopen(LOG_FILE, "w");

    if (out_file == NULL)
    {
        get_time(date_str);
        fprintf(stderr, "%s :: %s\n", date_str, "CANNOT OPEN LOG FILE");
        free(msg);
        free(date_str);
        return NULL;
    }

    while (atomic_load(&g_shared_data.running[thread_num]))
    {   
        
        if (th_rb_pop_front(&g_shared_data.th_rb_for_logger, msg,
                            &g_shared_data.running[thread_num]) != 0)
        {
            break;
        }
        
        get_time(date_str);
        fprintf(out_file, "%s :: %s\n", date_str, msg);
    }

    free(msg);
    get_time(date_str);
    fprintf(out_file, "%s :: %s", date_str, "EXITING LOGGER\n");
    free(date_str);
    fclose(out_file);

    return NULL;
}
#endif
