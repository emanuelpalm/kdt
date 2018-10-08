/**
 * Arbitrary length bit set utilities.
 *
 * @file
 */
#ifndef KDT_BITSET_H
#define KDT_BITSET_H

#include <kdt/mtx.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct bitset_t bitset_t;

/**
 * A collection of individually addressable bits.
 */
struct bitset_t {
    /// Pointer to beginning of bit set.
    uint8_t *bytes;

    /// Size of bit set, in bytes.
    size_t size;

    /// Lock.
    mtx_t lock;
};

/**
 * Initializes `bitset`.
 *
 * @param bitset Pointer to uninitialized bit set.
 * @param bytes Pointer to beginning of bit set memory.
 * @param size Size of bit set memory, in bytes.
 *
 * @note Not thread-safe.
 */
void bitset_Init(bitset_t *bitset, uint8_t *bytes, size_t size);

/**
 * Finds a free bit in `bitset`, clears it, and writes its position to `index`.
 *
 * @note Set (1) bits are considered free, while cleared (0) bits are
 * considered already allocated.
 *
 * @note Thread-safe.
 *
 * @param bitset Pointer to bit set.
 * @param index Pointer to allocated bit index receiver.
 * @return Whether or not there existed a free bit in `bitset`.
 */
bool bitset_Allocate(bitset_t *bitset, size_t *index);

/**
 * Clears `bitset` bit at `index`.
 *
 * @note It is the responsibility of the caller to ensure `index` is not out of
 * bounds.
 *
 * @note Thread-safe.
 *
 * @param bitset Pointer to bit set.
 * @param index Index of bit to clear (0).
 */
void bitset_Clear(bitset_t *bitset, size_t index);

/**
 * Sets `bitset` bit at `index`.
 *
 * @note It is the responsibility of the caller to ensure `index` is not out of
 * bounds.
 *
 * @note Thread-safe.
 *
 * @param bitset Pointer to bit set.
 * @param index Index of bit to set(1).
 */
void bitset_Set(bitset_t *bitset, size_t index);

#endif
