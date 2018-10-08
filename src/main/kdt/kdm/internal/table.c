#include "table.h"
#include <assert.h>
#include <string.h>

inline
void _kdm_InitTable(_kdm_Table *table, const kint_t *id) {
    assert(table != NULL);

    memset(table->buckets, 0, sizeof(_kdm_Bucket) * KDT_B1);
    if (id != NULL) {
        memcpy(&table->id, id, sizeof(kint_t));
    }
}

_kdm_Bucket *_kdm_GetBucket(_kdm_Table *table, const kint_t *id) {
    assert(table != NULL);
    assert(id != NULL);

    const kint_t distance = kint_XOR(&table->id, id);
    size_t index = kint_CLZ(&distance);
    if (index == KDT_B) {
        index -= 1;
    }
    if (index >= KDT_B1) {
        index -= (KDT_B - KDT_B1);
    }
    return &table->buckets[index];
}

inline
_kdm_Cursor _kdm_GetBucketCursor(_kdm_Table *table, const kint_t *id) {
    assert(table != NULL);
    assert(id != NULL);

    return (_kdm_Cursor) {
        .begin = &table->buckets[0],
        .offset = _kdm_GetBucket(table, id),
        .end = &table->buckets[KDT_B1],
    };
}