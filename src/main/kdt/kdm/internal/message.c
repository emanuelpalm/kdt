#include "message.h"

const char *_kdm_MessageTagAsString(uint16_t tag) {
    switch (tag) {
    case _KDM_MESSAGE_TAG_NONE:
        return "NONE";
    case _KDM_MESSAGE_TAG_ERROR:
        return "ERROR";
    case _KDM_MESSAGE_TAG_FIND_NODE:
        return "FIND_NODE";
    case _KDM_MESSAGE_TAG_FIND_VALUE:
        return "FIND_VALUE";
    case _KDM_MESSAGE_TAG_NODES:
        return "NODES";
    case _KDM_MESSAGE_TAG_PING:
        return "PING";
    case _KDM_MESSAGE_TAG_PONG:
        return "PONG";
    case _KDM_MESSAGE_TAG_STORE:
        return "STORE";
    case _KDM_MESSAGE_TAG_VALUE:
        return "VALUE";
    default:
        return "Unknown";
    }
}