#ifndef KDT_PNET_INTERNAL_EVENT_H
#define KDT_PNET_INTERNAL_EVENT_H

#include "../event.h"
#include "socket.h"
#include <stddef.h>
#include <stdint.h>

typedef struct _pnet_Event _pnet_Event;

struct _pnet_Event {
    /// Inner event.
    pnet_Event event;

    /// Position of event in event table.
    size_t index;

    /// Associated socket.
    _pnet_Socket socket;

    /// Number of bytes received.
    size_t bytes_received;

    /// Event body buffer.
    uint8_t data[KDT_N_BUFFER_SIZE];
};

static inline _pnet_Event *_pnet_AsPrivateEvent(pnet_Event *event) {
    return (_pnet_Event *) event;
}

static inline pnet_Event *_pnet_AsPublicEvent(pnet_EventMessage *event) {
    return (pnet_Event *) event;
}


#endif
