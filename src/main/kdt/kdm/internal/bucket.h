#ifndef KDT_KDM_INTERNAL_BUCKET_H
#define KDT_KDM_INTERNAL_BUCKET_H

#include "kdt/kdm/contact.h"
#include <kdt/def.h>

typedef struct _kdm_Bucket _kdm_Bucket;

/**
 * A Kademlia routing table bucket.
 */
struct _kdm_Bucket {
    /**
     * Bucket contacts, where the contact at index 0 is the most recently
     * contacted. If index 0 contains an empty contact, the entire list is to
     * be considered empty.
     */
    kdm_Contact contacts[KDT_K];
};

/**
 * Gets pointer to contact at index (KDM_K - 1) in `bucket`, if any.
 *
 * If any other pointer than NULL is returned, the bucket is full.
 *
 * @param bucket Pointer to bucket.
 * @return Pointer contact, or NULL.
 */
kdm_Contact *_kdm_GetKthContact(_kdm_Bucket *bucket);

/**
 * Pushes copy of `contact` into `bucket`.
 *
 * If the pushed contact is not already in the bucket, the contact is inserted
 * at index 0, which causes the contact at index (KDM_K - 1) to be dropped. If,
 * on the other hand, the pushed contact already exists in the bucket, it is
 * moved to index 0.
 *
 * The provided `contact` pointer MUST NOT point into `bucket`.
 *
 * @param bucket Pointer to bucket.
 * @param contact Pointer to contact to insert.
 */
void _kdm_PushContact(_kdm_Bucket *bucket, const kdm_Contact *contact);

/**
 * Removes contact with given `id` from `bucket`.
 *
 * If no contact with given ID exists in the bucket, the function does nothing.
 *
 * @param bucket Pointer to bucket.
 * @param id Pointer to Kademlia ID.
 */
void _kdm_RemoveContactWithID(_kdm_Bucket *bucket, const kint_t *id);

#endif
