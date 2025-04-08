#ifndef GB_h
#define GB_h

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* For SerenityOS compatibility */
#ifdef SERENITY
struct timespec {
    time_t tv_sec;
    long   tv_nsec;
};
#endif

#include <time.h>

#endif 