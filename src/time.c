#include "time.h"

void get_time_timeval(struct timeval *tv)
{
    gettimeofday(tv, NULL);
}

int64_t get_time_miliseconds(void)
{
    struct timeval tv;
    int64_t miliseconds;

    gettimeofday(&tv, NULL);
    miliseconds = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return miliseconds;
}

void miliseconds_to_timeval(int64_t miliseconds, struct timeval *tv)
{
    tv->tv_sec = miliseconds / 1000;
    tv->tv_usec = (miliseconds % 1000) * 1000;
}

void add_time_val_miliseconds(struct timeval *tv, int64_t miliseconds)
{
    get_time_timeval(tv);
    tv->tv_sec += miliseconds / 1000;
    tv->tv_usec += (miliseconds % 1000) * 1000;
    
    if(tv->tv_usec > 1000000)
    {
        tv->tv_sec += tv->tv_usec / 1000000;
        tv->tv_usec %= 1000000;
    } 
}
