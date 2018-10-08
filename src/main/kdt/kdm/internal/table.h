#ifndef KDT_KDM_INTERNAL_TABLE_H
#define KDT_KDM_INTERNAL_TABLE_H

#include "bucket.h"
#include "kdt/kdm/contact.h"
#include "cursor.h"
#include <kdt/def.h>
#include <stdbool.h>

typedef struct _kdm_Table _kdm_Table;

/**
 * A Kademlia routing table.
 *
 * Note that a table contains all of its owned memory; it does not reference it.
 * This means that a table requires a fairly large amount of memory. Avoid
 * creating transient tables. Also, any table function may cause data in the
 * table to move. Do not keep pointers into routing table buckets.
 */
struct _kdm_Table {
    /**
     * Routing table buckets.
     *
     * Contacts belong to certain buckets depending on the number of leading
     * zeroes in the binary forms of their distances to the table origin. A
     * distance with no leading zeroes belongs to the bucket at index 0, while
     * a distance with 1 leading zero belongs at index 1, and so on. This means
     * that the closer a bucket is to index (KDM_B1 - 1), the closer its
     * contacts are to the table `origin`. It also means that the further away
     * a contact is from the origin, the more likely it is to be be lost from
     * the table when updated.
     */
    _kdm_Bucket buckets[KDT_B1];

    /**
     * Table origin ID.
     *
     * The positions of table contacts are calculated in relation to their
     * logical distances to this ID.
     */
    kint_t id;
};

/**
 * Initializes `table` memory and sets table origin ID.
 *
 * @param table Pointer to routing table.
 * @param id Pointer to Kademlia ID.
 */
void _kdm_InitTable(_kdm_Table *table, const kint_t *id);

/**
 * Gets `table` bucket responsible for storing contact with given `id`.
 *
 * @param table Pointer to routing table.
 * @param id Pointer to Kademlia ID.
 * @return Pointer to bucket. Never returns NULL.
 */
_kdm_Bucket *_kdm_GetBucket(_kdm_Table *table, const kint_t *id);

/**
 * Gets `table` cursor positioned at bucket responsible for storing contact
 * with given `id`.
 *
 * @param table Pointer to routing table.
 * @param id Pointer to Kademlia ID.
 * @return Cursor. Never contains NULL pointers.
 */
_kdm_Cursor _kdm_GetBucketCursor(_kdm_Table *table, const kint_t *id);

#endif
