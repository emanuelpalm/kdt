#include "header.h"
#include "sender.h"
#include "server.h"
#include "socket.h"
#include "socket_set.h"
#include <assert.h>
#include <errno.h>
#include <string.h>

inline
void _pnet_InitSender(_pnet_Sender *sender) {
    size_t size = _BUFFER_O_COUNT_SIZE_T * sizeof(size_t);
    bitset_Init(&sender->allocations, (uint8_t *) sender->_allocations, size);
    memset(sender->_allocations, 0xff, size);

    const size_t count = KDT_N_BUFFER_O_COUNT;
    cbufz_Init(&sender->queue, sender->_queue, count);
}

_pnet_Message *_pnet_AllocateMessage(_pnet_Sender *sender) {
    size_t index;
    if (!bitset_Allocate(&sender->allocations, &index)) {
        return NULL;
    }
    _pnet_Message *_message = &sender->buffer[index];
    memset(&_message->message, 0, sizeof(pnet_Message));
    _message->message.data = mem_FromBuffer(_message->data, KDT_N_BUFFER_SIZE);
    _message->index = index;
    _message->socket = SOCKET_EMPTY;
    _message->bytes_sent = 0;
    _message->timeout = tims_Now() + 1.0;
    return _message;
}

inline
bool _pnet_PushMessage(_pnet_Sender *sender, _pnet_Message *message) {
    return cbufz_Push(&sender->queue, message->index);
}

static
void SendOne(_pnet_Sender *sender, _pnet_Server *server, _pnet_Message *message);

err_t _pnet_SendOutgoing(_pnet_Sender *sender, _pnet_Server *server) {
    assert(sender != NULL);
    assert(server != NULL);

    err_t err;

    _pnet_SocketSet socket_set;
    if ((err = _pnet_PollWritableSockets(server, &socket_set)) != ERR_NONE) {
        return err;
    }

    for (size_t i = KDT_N_BUFFER_O_COUNT; i-- != 0;) {
        size_t index;
        if (!cbufz_Pop(&sender->queue, &index)) {
            break;
        }
        _pnet_Message *message = &sender->buffer[index];

        if (_pnet_IsSocketEmpty(&message->socket)) {
            _pnet_Socket socket;
            err = _pnet_Connect(server, &message->message.receiver, &socket);
            if (err != ERR_NONE) {
                goto handle_error;
            }
            message->socket = socket;
        }
        else if (!_pnet_IsSocketReady(&socket_set, &message->socket)) {
            if (tims_Now() < message->timeout) {
                cbufz_Push(&sender->queue, message->index);
            } else {
                err = ERR_TIMEOUT;
                goto handle_error;
            }
            continue;
        }
        SendOne(sender, server, message);
        continue;

handle_error:
        _pnet_HandleError(server, &(_pnet_Error) {
            .nonce = &message->message.nonce,
            .host = &message->message.receiver,
            .tag = message->message.tag,
            .err = err,
        });
    }

    return ERR_NONE;
}

#define _TRY(ERR) do {               \
    if ((err = (ERR)) != ERR_NONE) { \
        goto error;                  \
    }                                \
} while (0)

static inline
void SendOne(_pnet_Sender *sender, _pnet_Server *server, _pnet_Message *message) {
    size_t bytes_remaining;
    size_t n;
    err_t err;

    // Send message header, if haven't already.
    if (message->bytes_sent < _PNET_HEADER_SIZE) {
        uint8_t buffer[_PNET_HEADER_SIZE];
        mem_t mem = mem_FromBuffer(buffer, _PNET_HEADER_SIZE);
        mem_Write(&mem, message->message.nonce.as_u8s, sizeof(kint_t));
        mem_WriteU16BE(&mem, message->message.tag);
        mem_WriteU16BE(&mem, (uint16_t) mem_Size(&message->message.data));

        n = _PNET_HEADER_SIZE - message->bytes_sent;
        _TRY(_pnet_Send(&message->socket, &n, &buffer[message->bytes_sent]));
        message->bytes_sent += n;

        if (n < _PNET_HEADER_SIZE) {
            goto requeue;
        }
    }

    // Send more of or all of message body.
    {
        bytes_remaining = _PNET_HEADER_SIZE + mem_Size(&message->message.data) -
                          message->bytes_sent;

        n = bytes_remaining;
        _TRY(_pnet_Send(&message->socket, &n,
                        &message->data[message->bytes_sent - _PNET_HEADER_SIZE]));
        message->bytes_sent += n;

        if (message->bytes_sent < bytes_remaining) {
            goto requeue;
        }
    }

disconnect:
    bitset_Set(&sender->allocations, message->index);
    _pnet_CloseSocket(server, &message->socket);
    return;

requeue:
    cbufz_Push(&sender->queue, message->index);
    return;

error:
    if (err == EAGAIN && tims_Now() < message->timeout) {
        goto requeue;
    }
    _pnet_HandleError(server, &(_pnet_Error) {
        .nonce = &message->message.nonce,
        .host = &message->message.receiver,
        .tag = message->message.tag,
        .err = err,
    });
    goto disconnect;
}

#undef _TRY