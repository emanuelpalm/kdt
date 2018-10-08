#ifndef KDT_MTX_H
#define KDT_MTX_H

#include <stdbool.h>

#ifdef KDT_USE_POSIX
#include <pthread.h>
#endif

typedef struct mtx_t mtx_t;

struct mtx_t {
#ifdef KDT_USE_POSIX
    pthread_mutex_t ptm;
#endif
};

void mtx_Init(mtx_t *mtx);
void mtx_Lock(mtx_t *mtx);
bool mtx_TryLock(mtx_t *mtx);
void mtx_Unlock(mtx_t *mtx);

#endif
