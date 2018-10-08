#include "mtx.h"

#ifdef KDT_USE_POSIX
#include <errno.h>
#include <kdt/log.h>

inline
void mtx_Init(mtx_t *mtx) {
    int status = pthread_mutex_init(&mtx->ptm, NULL);
    if (status != 0) {
        log_PanicF("Failed to initialize PHREAD mutex; status: %d", status);
    }
}

inline
void mtx_Lock(mtx_t *mtx) {
    int status = pthread_mutex_lock(&mtx->ptm);
    if (status != 0) {
        log_PanicF("Unexpected PTHREAD mutex status: %d", status);
    }
}

inline
bool mtx_TryLock(mtx_t *mtx) {
    int status = pthread_mutex_trylock(&mtx->ptm);
    switch (status) {
    case 0:
        return true;
    case EBUSY:
        return false;
    default:
        log_PanicF("Unexpected PTHREAD mutex status: %d", status);
    }
}

inline
void mtx_Unlock(mtx_t *mtx) {
    int status = pthread_mutex_unlock(&mtx->ptm);
    if (status != 0) {
        log_PanicF("Unexpected PTHREAD mutex status: %d", status);
    }
}

#else
#error No supportex MTX implementation.
#endif