#ifndef _TIME_H
#define _TIME_H

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

void get_time_timeval(struct timeval *);
int64_t get_time_miliseconds(void);
void miliseconds_to_timeval(int64_t, struct timeval *);
void add_timeval_miliseconds(struct timeval *, int64_t);

#endif
