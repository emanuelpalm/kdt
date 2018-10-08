#ifndef KDT_PNET_INTERNAL_SENDER_H
#define KDT_PNET_INTERNAL_SENDER_H

#include "message.h"
#include <kdt/bitset.h>
#include <kdt/cbuf.h>

#define _BUFFER_O_COUNT_SIZE_T \
    ((KDT_N_BUFFER_O_COUNT / (sizeof(size_t) * 8)) + \
     ((KDT_N_BUFFER_O_COUNT % (sizeof(size_t) * 8)) == 0 ? 0 : 1))

typedef struct _pnet_Sender _pnet_Sender;
typedef struct _pnet_Server _pnet_Server;

struct _pnet_Sender {
    /// Message buffer.
    _pnet_Message buffer[KDT_N_BUFFER_O_COUNT];

    /// Bit set for keeping track of message buffer allocations.
    bitset_t allocations;

    /// Queue with buffer indexes of outgoing messages.
    cbufz_t queue;

    /// Backing memory for bit set.
    size_t _allocations[_BUFFER_O_COUNT_SIZE_T];

    /// Backing memory for queue.
    size_t _queue[KDT_N_BUFFER_O_COUNT];
};

void _pnet_InitSender(_pnet_Sender *sender);
_pnet_Message *_pnet_AllocateMessage(_pnet_Sender *sender);
bool _pnet_PushMessage(_pnet_Sender *sender, _pnet_Message *message);
err_t _pnet_SendOutgoing(_pnet_Sender *sender, _pnet_Server *server);

#endif
