#include "bucket.h"
#include <assert.h>
#include <string.h>

inline
kdm_Contact *_kdm_GetKthContact(_kdm_Bucket *bucket) {
    kdm_Contact *contact = &bucket->contacts[KDT_K - 1];
    return kdm_IsContactEmpty(contact) ? NULL : contact;
}

bool ResolveIndex(const _kdm_Bucket *bucket, const kint_t *id, size_t *out);

void _kdm_PushContact(_kdm_Bucket *bucket, const kdm_Contact *contact) {
    assert(contact < &bucket->contacts[0] || contact >= &bucket->contacts[KDT_K]);

    size_t size;
    {
        size_t index;
        if (ResolveIndex(bucket, &contact->id, &index)) {
            if (index == 0) {
                return;
            }
        }
        else {
            index = (KDT_K - 1);
        }
        size = index * sizeof(kdm_Contact);
    }
    memmove(&bucket->contacts[1], &bucket->contacts[0], size);
    memcpy(&bucket->contacts[0], contact, sizeof(kdm_Contact));
}

void _kdm_RemoveContactWithID(_kdm_Bucket *bucket, const kint_t *id) {
    size_t index;
    if (ResolveIndex(bucket, id, &index)) {
        if (index < (KDT_K - 1)) {
            const size_t size = (KDT_K - 1 - index) * sizeof(kdm_Contact);
            memmove(&bucket->contacts[index], &bucket->contacts[index + 1], size);
        }
        memset(&bucket->contacts[KDT_K - 1], 0, sizeof(kdm_Contact));
    }
}

bool ResolveIndex(const _kdm_Bucket *bucket, const kint_t *id, size_t *out) {
    size_t i = 0;
    do {
        const kdm_Contact *contact = &bucket->contacts[i];
        if (kdm_IsContactEmpty(contact)) {
            break;
        }
        if (kint_EQU(id, &contact->id)) {
            *out = i;
            return true;
        }
    } while (++i < KDT_K);
    return false;
}