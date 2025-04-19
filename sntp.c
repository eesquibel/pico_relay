#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/util/datetime.h"
#include "pico/aon_timer.h"

#ifdef SNTP_SERVER
#include "sntp.h"

void sntp_set_system_time(u32_t sec)
{
    printf("SNTP time: %u\n", sec);

    struct timespec current_ts = {0};
    current_ts.tv_sec = sec;
    current_ts.tv_nsec = 0;
    
    if (aon_timer_is_running())
    {
        aon_timer_set_time(&current_ts);
    }
    else
    {
        aon_timer_start(&current_ts);
    }

    struct tm current_tm;
    aon_timer_get_time_calendar(&current_tm);

    printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
           current_tm.tm_year + 1900, current_tm.tm_mon + 1, current_tm.tm_mday,
           current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec);
}
#endif
