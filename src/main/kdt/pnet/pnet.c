#include "pnet.h"
#include <assert.h>
#include <errno.h>
#include <kdt/bitset.h>
#include <kdt/cbuf.h>
#include <kdt/def.h>
#include <kdt/log.h>

static
void OnServerError(_pnet_Error *error, void *data);

err_t pnet_Open(pnet_t *pnet, pnet_Host *interface) {
    _pnet_OnError on_error = {
        .callback = OnServerError,
        .data = pnet,
    };
    err_t err;
    if ((err = _pnet_Open(&pnet->server, interface, on_error)) != ERR_NONE) {
        return err;
    }
    _pnet_InitSender(&pnet->sender);
    _pnet_InitReceiver(&pnet->receiver);
    return ERR_NONE;
}

inline
void pnet_Close(pnet_t *pnet) {
    _pnet_Close(&pnet->server);
}

inline
const pnet_Host *pnet_GetInterface(pnet_t *pnet) {
    return pnet->server.interface;
}

/*
 * The thread-safety of this function comes from its use of thread-safe queues
 * and never allowing more than one thread to poll the network interface for
 * new connections, data or send buffer space.
 *
 * When the first thread enters this function, it will find the event queue to
 * be empty, and then proceed to enter a critical region in which messages are
 * sent and received. Any received messages, or errors, are put in the event
 * queue, while sent messages are taken from the outbound message queue and
 * transmitted. When exiting the critical region, a final attempt is made to
 * take an event from the event queue.
 *
 * If another thread calls the function while the first is still inside the
 * critical region, it will return either with an event taken from the queue,
 * or with the admonition the try the function again later, if the queue was
 * empty.
 */
err_t pnet_Poll(pnet_t *pnet, pnet_Event **out) {
    err_t err;
    _pnet_Event *event;

    // If there is a received but unhandled event, return one immediately.
    if ((event = _pnet_PopReceivedEvent(&pnet->receiver)) != NULL) {
        *out = &event->event;
        return ERR_NONE;
    }

    if (!mtx_TryLock(&pnet->lock)) {
        return ERR_TRY_AGAIN;
    }
    {
        // Send any pending outbound messages.
        if ((err = _pnet_SendOutgoing(&pnet->sender, &pnet->server)) != ERR_NONE) {
            goto leave_critical;
        }

        // Receive any pending inbound messages and accept incoming connections.
        if ((err = _pnet_ReceiveIncoming(&pnet->receiver, &pnet->server)) != ERR_NONE) {
            goto leave_critical;
        }
    }
leave_critical:
    mtx_Unlock(&pnet->lock);
    if (err != ERR_NONE) {
        return err;
    }

    // Try to claim an unhandled event, again.
    if ((event = _pnet_PopReceivedEvent(&pnet->receiver)) != NULL) {
        *out = &event->event;
    }
    else {
        *out = NULL;
    }

    return ERR_NONE;
}

inline
err_t pnet_Send(pnet_t *pnet, pnet_Message *message) {
    assert(message != NULL);

    _pnet_Message *_message = _pnet_AsPrivateMessage(message);
    if (!_pnet_PushMessage(&pnet->sender, _message)) {
        return ENOMEM;
    }
    return ERR_NONE;
}

inline
pnet_Message *pnet_NewMessage(pnet_t *pnet) {
    _pnet_Message *_message = _pnet_AllocateMessage(&pnet->sender);
    return _message != NULL ? &_message->message : NULL;
}

inline
pnet_Message *pnet_NewResponse(pnet_t *pnet, pnet_EventMessage *request) {
    assert(request != NULL);

    _pnet_Event *_event = _pnet_AsPrivateEvent(_pnet_AsPublicEvent(request));
    _pnet_Message *_message = _pnet_AllocateMessage(&pnet->sender);
    if (_message == NULL) {
        return NULL;
    }
    _message->socket = _event->socket;
    _event->socket = SOCKET_EMPTY; // Prevent socket from being closed when event is freed.
    return &_message->message;
}

inline
void pnet_FreeEvent(pnet_t *pnet, pnet_Event *event) {
    assert(event != NULL);

    _pnet_FreeEvent(&pnet->receiver, _pnet_AsPrivateEvent(event));
    _pnet_Event *_event = _pnet_AsPrivateEvent(event);
    if (!_pnet_IsSocketEmpty(&_event->socket)) {
        _pnet_CloseSocket(&pnet->server, &_event->socket);
    }
}

static
void OnServerError(_pnet_Error *error, void *data) {
    pnet_t *pnet = data;
    _pnet_Event *event = _pnet_AllocateEvent(&pnet->receiver);
    if (event == NULL) {
        log_Warn("No receiver buffer is available for storing error event.");
        return;
    }
    event->event.as_error = (pnet_EventError) {
        .type = PNET_EVENT_ERROR,
        .nonce = (error->nonce != NULL) ? *error->nonce : (kint_t) {0},
        .host = (error->host != NULL) ? *error->host : (pnet_Host) {0},
        .tag = error->tag,
        .code = error->err,
    };
    _pnet_PushEvent(&pnet->receiver, event);
}