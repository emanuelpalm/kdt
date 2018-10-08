#include "cbuf.h"
#include <assert.h>

inline
void cbufz_Init(cbufz_t *cbuf, size_t *origin, size_t capacity) {
    assert(cbuf != NULL);
    assert(origin != NULL);
    assert(capacity > 0);

    cbuf->origin = origin;
    cbuf->capacity = capacity;
    cbuf->read = 0;
    cbuf->write = 0;
    mtx_Init(&cbuf->lock);
}

inline
bool cbufz_Push(cbufz_t *cbuf, size_t element) {
    assert(cbuf != NULL);

    bool status = true;

    mtx_Lock(&cbuf->lock);
    {
        size_t write = (cbuf->write + 1) % cbuf->capacity;
        if (write == cbuf->read) {
            status = false;
            goto leave;
        }
        cbuf->origin[cbuf->write] = element;
        cbuf->write = write;
    }
leave:
    mtx_Unlock(&cbuf->lock);
    return status;
}

inline
bool cbufz_Pop(cbufz_t *cbuf, size_t *out) {
    assert(cbuf != NULL);
    assert(out != NULL);

    size_t element;

    mtx_Lock(&cbuf->lock);
    {
        if (cbuf->read == cbuf->write) {
            mtx_Unlock(&cbuf->lock);
            return false;
        }
        element = cbuf->origin[cbuf->read];
        cbuf->read = (cbuf->read + 1) % cbuf->capacity;
    }
    mtx_Unlock(&cbuf->lock);

    *out = element;
    return true;
}