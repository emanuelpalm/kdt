#include "options.h"
#include <errno.h>
#include <stdlib.h>

#define _TRY(ERR) do {         \
    const err_t _err = (ERR); \
    if (_err != ERR_NONE) {   \
        return _err;          \
     }                        \
} while (0)

err_t options_FromEnvironment(options_t *out) {
    mem_t interface = mem_FromString(getenv("KDT_INTERFACE"));
    if (mem_Space(&interface) > 0) {
        _TRY(pnet_ReadHostText(&interface, &out->interface));
    }

    mem_t peer = mem_FromString(getenv(("KDT_PEER")));
    if (mem_Space(&peer) > 0) {
        _TRY(pnet_ReadHostText(&peer, &out->peer));
    }

    return ERR_NONE;
}