#ifndef KDT_PNET_INTERNAL_RECEIVER_H
#define KDT_PNET_INTERNAL_RECEIVER_H

#include "event.h"
#include <kdt/bitset.h>
#include <kdt/cbuf.h>

#define _BUFFER_I_COUNT_SIZE_T \
    ((KDT_N_BUFFER_I_COUNT / (sizeof(size_t) * 8)) + \
     ((KDT_N_BUFFER_I_COUNT % (sizeof(size_t) * 8)) == 0 ? 0 : 1))

typedef struct _pnet_Receiver _pnet_Receiver;
typedef struct _pnet_Server _pnet_Server;

struct _pnet_Receiver {
    /// Event buffer.
    _pnet_Event buffer[KDT_N_BUFFER_I_COUNT];

    /// Bit set for keeping track of event buffer allocations.
    bitset_t allocations;

    /// Queue with buffer indexes of fully received events.
    cbufz_t queue_ready;

    /// Queue with buffer indexes of partially received events.
    cbufz_t queue_unready;

    /// Backing memory for bit set.
    size_t _allocations[_BUFFER_I_COUNT_SIZE_T];

    /// Backing memory for ready queue.
    size_t _queue_ready[KDT_N_BUFFER_I_COUNT];

    /// Backing memory for unready queue.
    size_t _queue_unready[KDT_N_BUFFER_I_COUNT];
};

void _pnet_InitReceiver(_pnet_Receiver *receiver);
_pnet_Event *_pnet_AllocateEvent(_pnet_Receiver *receiver);
void _pnet_FreeEvent(_pnet_Receiver *receiver, _pnet_Event *event);
_pnet_Event *_pnet_PopReceivedEvent(_pnet_Receiver *receiver);
bool _pnet_PushEvent(_pnet_Receiver *receiver, _pnet_Event *event);
err_t _pnet_ReceiveIncoming(_pnet_Receiver *receiver, _pnet_Server *server);

#endif
