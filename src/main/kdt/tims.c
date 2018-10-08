#include "tims.h"

#ifdef KDT_USE_POSIX

#include <time.h>

inline
tims_t tims_Now() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((tims_t) ts.tv_sec) + (((tims_t) ts.tv_nsec) / 1000000000.0);
}

#else
#error No supported TIMS implementation.
#endif