#ifndef KDT_KDM_INTERNAL_MESSAGE_H
#define KDT_KDM_INTERNAL_MESSAGE_H

#include <stdint.h>

// TODO: Make sure all these messages (exception NONE) can be handled properly.
enum {
    _KDM_MESSAGE_TAG_NONE = 0,
    _KDM_MESSAGE_TAG_ERROR = 1,
    _KDM_MESSAGE_TAG_FIND_NODE = 2,
    _KDM_MESSAGE_TAG_FIND_VALUE = 3,
    _KDM_MESSAGE_TAG_NODES = 4,
    _KDM_MESSAGE_TAG_PING = 5,
    _KDM_MESSAGE_TAG_PONG = 6,
    _KDM_MESSAGE_TAG_STORE = 7,
    _KDM_MESSAGE_TAG_VALUE = 8,
};

const char *_kdm_MessageTagAsString(uint16_t tag);

#endif
