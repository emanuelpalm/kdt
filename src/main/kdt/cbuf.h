/**
 * Arbitrary length circular index buffer utilities.
 *
 * @file
 */
#ifndef KDT_CBUF_H
#define KDT_CBUF_H

#include <kdt/mtx.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct cbufz_t cbufz_t;

/**
 * A fixed-size circular buffer containing `size_t` elements.
 *
 * @note A circular buffer can only fit one element less than its `capacity`.
 * Also, the capacity of a buffer must not be less than one.
 */
struct cbufz_t {
    /// Pointer to origin element in buffer.
    size_t *origin;

    /// Buffer capacity, in elements.
    size_t capacity;

    /// Index of next element to be written to.
    size_t write;

    /// Index of next element to be read from.
    size_t read;

    /// Lock.
    mtx_t lock;
};

/**
 * Initializes `cbuf`.
 *
 * @note Not thread-safe.
 *
 * @param cbuf Pointer to uninitialized circular buffer.
 * @param origin Pointer to beginning of buffer memory.
 * @param capacity Buffer memory capacity, in elements.
 */
void cbufz_Init(cbufz_t *cbuf, size_t *origin, size_t capacity);

/**
 * Attempts to push `element` into `cbuf`.
 *
 * @note Thread-safe.
 *
 * @param cbuf Pointer to circular buffer.
 * @param element Element to enqueue.
 * @return Whether or not there was room for element.
 */
bool cbufz_Push(cbufz_t *cbuf, size_t element);

/**
 * Attempts to pop element from `cbuf` to `out`.
 *
 * @note Thread-safe.
 *
 * @param cbuf Pointer to circular buffer.
 * @param out Receiver of dequeued element.
 * @return Whether or not there was any element to pop.
 */
bool cbufz_Pop(cbufz_t *cbuf, size_t *out);

#endif
