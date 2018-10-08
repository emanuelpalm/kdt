#include "header.h"
#include "receiver.h"
#include "server.h"
#include "socket.h"
#include "socket_set.h"
#include <assert.h>
#include <errno.h>
#include <kdt/endian.h>
#include <kdt/log.h>
#include <string.h>

typedef struct _Context _Context;

struct _Context {
    _pnet_Receiver *receiver;
    _pnet_Server *server;
};

inline
void _pnet_InitReceiver(_pnet_Receiver *receiver) {
    const size_t size = _BUFFER_I_COUNT_SIZE_T * sizeof(size_t);
    bitset_Init(&receiver->allocations, (uint8_t *) receiver->_allocations, size);
    memset(receiver->_allocations, 0xff, size);

    const size_t count = KDT_N_BUFFER_I_COUNT;
    cbufz_Init(&receiver->queue_ready, receiver->_queue_ready, count);
    cbufz_Init(&receiver->queue_unready, receiver->_queue_unready, count);
}

inline
_pnet_Event *_pnet_AllocateEvent(_pnet_Receiver *receiver) {
    size_t index;
    if (!bitset_Allocate(&receiver->allocations, &index)) {
        return NULL;
    }
    _pnet_Event *event = &receiver->buffer[index];
    memset(&event->event, 0, sizeof(pnet_Event));
    event->index = index;
    event->socket = SOCKET_EMPTY;
    event->bytes_received = 0;
    return event;
}

inline
void _pnet_FreeEvent(_pnet_Receiver *receiver, _pnet_Event *event) {
    bitset_Set(&receiver->allocations, event->index);
}

_pnet_Event *_pnet_PopReceivedEvent(_pnet_Receiver *receiver) {
    size_t index;
    if (cbufz_Pop(&receiver->queue_ready, &index)) {
        return &receiver->buffer[index];
    }
    return NULL;
}

inline
bool _pnet_PushEvent(_pnet_Receiver *receiver, _pnet_Event *event) {
    return cbufz_Push(&receiver->queue_ready, event->index);
}

static
bool OnSocketReady(void *context, _pnet_Socket *socket);

static
void ReceiveOne(_Context *context, _pnet_Event *event);

err_t _pnet_ReceiveIncoming(_pnet_Receiver *receiver, _pnet_Server *server) {
    assert(receiver != NULL);
    assert(server != NULL);

    err_t err;
    _Context context = {.receiver = receiver, .server = server};
    bool pending_accepts = false;

    _pnet_SocketSet socket_set;
    if ((err = _pnet_PollReadableSockets(server, &socket_set)) != ERR_NONE) {
        return err;
    }

    // Handle data for existing/unready events.
    while (true) {
        size_t index;
        if (!cbufz_Pop(&receiver->queue_unready, &index)) {
            break;
        }
        _pnet_Event *event = &receiver->buffer[index];
        if (!_pnet_IsSocketReady(&socket_set, &event->socket)) {
            cbufz_Push(&receiver->queue_unready, event->index);
            continue;
        }
        _pnet_ClearSocket(&socket_set, &event->socket);
        ReceiveOne(&context, event);
    }

    // Handle data for new events.
    _pnet_ForEachReadySocket(&socket_set, &pending_accepts, &context, OnSocketReady);

    return pending_accepts
        ? _pnet_Accept(server)
        : ERR_NONE;
}

static
bool OnSocketReady(void *context, _pnet_Socket *socket) {
    _Context *_context = context;

    _pnet_Event *event = _pnet_AllocateEvent(_context->receiver);
    if (event == NULL) {
        log_Warn("All receiver message buffers are full.");
        return false;
    }
    event->socket = *socket;
    ReceiveOne(_context, event);
    return true;
}

#define _TRY(ERR) do {               \
    if ((err = (ERR)) != ERR_NONE) { \
        goto error;                  \
    }                                \
} while (0)

static
void ReceiveOne(_Context *context, _pnet_Event *event) {
    pnet_EventMessage *message = &event->event.as_message;
    size_t bytes_remaining;
    size_t n;
    err_t err;

    // Get IP address of sending host, if haven't already.
    if (event->bytes_received == 0) {
        _TRY(_pnet_ResolveHost(context->server, &event->socket,
                               &event->event.as_message.sender));
    }

    // Receive message header, if haven't already.
    if (event->bytes_received < _PNET_HEADER_SIZE) {
        bytes_remaining = _PNET_HEADER_SIZE - event->bytes_received;

        n = bytes_remaining;
        _TRY(_pnet_Receive(&event->socket, &n, &event->data[event->bytes_received]));
        event->bytes_received += n;

        if (event->bytes_received < _PNET_HEADER_SIZE) {
            goto requeue;
        }

        mem_t mem = mem_FromBuffer(event->data, _PNET_HEADER_SIZE);
        mem_Read(&mem, sizeof(kint_t), message->nonce.as_u8s);
        mem_ReadU16BE(&mem, &message->tag);
        uint16_t size;
        if (!mem_ReadU16BE(&mem, &size)) {
            size = 0;
        }
        message->data = mem_FromBuffer(event->data, size);
    }

    // Receive more of or all of message body.
    {
        bytes_remaining = _PNET_HEADER_SIZE + mem_Size(&message->data) -
                          event->bytes_received;

        n = bytes_remaining;
        _TRY(_pnet_Receive(&event->socket, &n, &event->data[event->bytes_received]));
        event->bytes_received += n;

        if (event->bytes_received < bytes_remaining) {
            goto requeue;
        }
    }

requeue:
    cbufz_Push(&context->receiver->queue_unready, event->index);
    return;

error:
    if (err == EAGAIN) {
        goto requeue;
    }
    _pnet_CloseSocket(context->server, &event->socket);

    _pnet_Error serr = {
        .err = err,
        .host = &event->event.as_message.sender,
    };
    if (event->bytes_received >= _PNET_HEADER_SIZE) {
        serr.nonce = &event->event.as_message.nonce;
        serr.tag = event->event.as_message.tag;
    }
    else {
        serr.nonce = NULL;
        serr.tag = 0;
    }
    _pnet_HandleError(context->server, &serr);
}

#undef _TRY