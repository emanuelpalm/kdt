/**
 * Peer-to-peer networking utilities.
 *
 * @file
 */
#ifndef KDT_PNET_H
#define KDT_PNET_H

#include "event.h"
#include "host.h"
#include "internal/event.h"
#include "internal/message.h"
#include "internal/receiver.h"
#include "internal/sender.h"
#include "internal/server.h"
#include "message.h"
#include <kdt/err.h>
#include <kdt/mtx.h>
#include <stdint.h>

typedef struct pnet_t pnet_t;

/**
 * Peer-to-peer networking node.
 */
struct pnet_t {
    /// Send/receive lock.
    mtx_t lock;

    /// Server for handling peer connectivity.
    _pnet_Server server;

    /// Buffers for outgoing messages.
    _pnet_Sender sender;

    /// Buffers for incoming messages.
    _pnet_Receiver receiver;
};

/**
 * Enables message sending and starts listening for incoming peer-to-peer
 * messages via the identified network `interface`.
 *
 * Any property of the referenced interface being 0 is considered expressing
 * either that all relevant choices are made, or that any suitable is made. The
 * interface is updated to reflect the concrete value, if any one particular,
 * before the function returns.
 *
 * If, for example, `{interface}->internet == PNET_INTERNET_IPV4` and
 * `{interface}->address` is zeroed, then an attempt will be made to listen on
 * all IPv4-compatible network interfaces. If, on the other hand, IPv4 is
 * desired but a 0 `{interface}->port` is provided, only one port will be
 * selected.
 *
 * @note `net_Close()` must be called when no more messages will be sent or
 * received.
 *
 * @note On POSIX systems, the implementation relies on the `select()` call,
 * which means that there is a limit to how many sockets it can keep open at a
 * given time. If multiple `pnet_t` instances are opened at the same time,
 * there is an increased risk of running out of sockets.
 *
 * @note Not thread-safe.
 *
 * @param pnet Pointer to uninitialized PNET structure.
 * @param port Pointer to interface.
 * @return ERR_NONE only if operation was successful.
 */
err_t pnet_Open(pnet_t *pnet, pnet_Host *interface);

/**
 * Disables message sending and stops listening for incoming peer-to-peer
 * messages.
 *
 * @note Must be called after `net_Open()`.
 *
 * @note Not thread-safe.
 *
 * @param pnet Pointer to initialized PNET structure.
 */
void pnet_Close(pnet_t *pnet);

/**
 * Gets pointer to interface used to open peer-to-peer network.
 *
 * @param pnet Pointer to PNET structure.
 * @return Pointer to host structure.
 */
const pnet_Host *pnet_GetInterface(pnet_t *pnet);

/**
 * Polls for one new inbound message, if any.
 *
 * `out` is set to NULL if no new inbound message is available. The intention
 * is for this function to be called repeatedly until such a NULL is observed,
 * perhaps by a pool of worker threads. If more than one thread is calling this
 * function, it may return ERR_TRY_AGAIN, which is meant to signal that it
 * isn't yet known if there are any new pending events and the caller should
 * try again after at least a few milliseconds.
 *
 * @note Every non-NULL pointer received from this function must be passed to
 * `pnet_FreeEvent()` once no longer in use.
 *
 * @note Calling this function before invoking `pnet_Open()` or after invoking
 * `pnet_Close()` causes undefined behavior.
 *
 * @note Thread-safe.
 *
 * @param pnet Pointer to PNET structure.
 * @param out Pointer to event.
 * @return ERR_NONE only if operation was successful.
 */
err_t pnet_Poll(pnet_t *pnet, pnet_Event **out);

/**
 * Enqueues message for being sent to network peer.
 *
 * The `nonce` provided in `message` will be visible either to the message
 * receiver, or in any error generated if the message could not be delivered.
 * If the send buffer is full, the function returns ERR_TRY_AGAIN, which means
 * that the send operation may be successful if tried again after a call to
 * `pnet_Poll()`.
 *
 * @note The `message` pointer must have been acquired via a call to
 * `pnet_NewMessage()`, or this function may cause undefined behavior.
 *
 * @note The function returns before it is known whether or not the message
 * could be sent. Any failure is made known via some subsequent call to
 * `net_Poll()`.
 *
 * @note Calling this function before invoking `pnet_Open()` or after invoking
 * `pnet_Close()` causes undefined behavior.
 *
 * @note Thread-safe.
 *
 * @param pnet Pointer to PNET structure.
 * @param message Pointer to message structure.
 * @return ERR_NONE only if operation was successful.
 */
err_t pnet_Send(pnet_t *pnet, pnet_Message *message);

/**
 * Allocates a free outbound message buffer.
 *
 * @note Calling this function before invoking `pnet_Open()` or after invoking
 * `pnet_Close()` causes undefined behavior.
 *
 * @note Thread-safe.
 *
 * @param pnet Pointer to PNET structure.
 * @return Pointer to allocated message buffer, or NULL if no is available.
 */
pnet_Message *pnet_NewMessage(pnet_t *pnet);

/**
 * Allocates a free outbound message buffer, intended to contain a reply to
 * given `request`.
 *
 * The difference between this function and `pnet_NewMessage()` is that it
 * may try to optimize by keeping sockets open, or similar.
 *
 * @note Calling this function before invoking `pnet_Open()` or after invoking
 * `pnet_Close()` causes undefined behavior.
 *
 * @note Thread-safe.
 *
 * @param pnet Pointer to PNET structure.
 * @param request Pointer to inbound message.
 * @return Pointer to allocated message buffer, of NULL if no is available.
 */
pnet_Message *pnet_NewResponse(pnet_t *pnet, pnet_EventMessage *request);

/**
 * Frees inbound message no longer in use.
 *
 * @note The `message` pointer must have been previously received via
 * `pnet_Poll()`.
 *
 * @note Calling this function before invoking `pnet_Open()` or after invoking
 * `pnet_Close()` causes undefined behavior.
 *
 * @note Thread-safe.
 *
 * @param pnet Pointer to PNET structure.
 * @param event Pointer to event.
 */
void pnet_FreeEvent(pnet_t *pnet, pnet_Event *event);

#endif
