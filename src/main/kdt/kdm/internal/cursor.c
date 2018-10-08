#include "bucket.h"
#include "cursor.h"

inline
bool _kdm_Forward(_kdm_Cursor *cursor) {
    _kdm_Bucket *offset = &cursor->offset[1];
    if (offset < cursor->end) {
        cursor->offset = offset;
        return true;
    }
    return false;
}

inline
bool _kdm_Rewind(_kdm_Cursor *cursor) {
    if (cursor->offset > cursor->begin) {
        cursor->offset = &cursor->offset[-1];
        return true;
    }
    return false;
}