#ifndef KDT_PNET_INTERNAL_MESSAGE_H
#define KDT_PNET_INTERNAL_MESSAGE_H

#include "../message.h"
#include "socket.h"
#include <kdt/tims.h>
#include <stddef.h>
#include <stdint.h>

typedef struct _pnet_Message _pnet_Message;

struct _pnet_Message {
    /// Inner message.
    pnet_Message message;

    /// Position of message in sender message table.
    size_t index;

    /// Associated socket, or SOCKET_NONE if not known.
    _pnet_Socket socket;

    /// Number of bytes sent of message, including header bytes.
    size_t bytes_sent;

    /// Time at which message sending times out and the message is discarded.
    tims_t timeout;

    /// Message body buffer.
    uint8_t data[KDT_N_BUFFER_SIZE];
};

static inline _pnet_Message *_pnet_AsPrivateMessage(pnet_Message *message) {
    return (_pnet_Message *) message;
}

#endif
