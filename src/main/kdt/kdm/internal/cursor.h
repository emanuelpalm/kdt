#ifndef KDT_KDM_INTERNAL_CURSOR_H
#define KDT_KDM_INTERNAL_CURSOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct _kdm_Bucket _kdm_Bucket;
typedef struct _kdm_Cursor _kdm_Cursor;

/**
 * Cursor for iterating through the buckets of a routing table.
 */
struct _kdm_Cursor {
    /// Pointer to first bucket in table.
    _kdm_Bucket *begin;

    /// Pointer greater than or equal to `begin` and lesser than `end`.
    _kdm_Bucket *offset;

    /// Pointer to first byte right after last bucket in table.
    const _kdm_Bucket *end;
};

/**
 * Moves `cursor` offset pointer one bucket closer to table end, if possible.
 *
 * @param cursor Pointer to cursor.
 * @return Whether or not cursor offset could be changed.
 */
bool _kdm_Forward(_kdm_Cursor *cursor);

/**
 * Moves `cursor` offset pointer one bucket closer to table start, if possible.
 *
 * @param cursor Pointer to cursor.
 * @return Whether or not cursor offset could be changed.
 */
bool _kdm_Rewind(_kdm_Cursor *cursor);

#endif