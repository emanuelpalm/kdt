#ifndef KDT_PNET_EVENT_H
#define KDT_PNET_EVENT_H

#include "host.h"
#include "message.h"
#include <kdt/kint.h>
#include <kdt/mem.h>
#include <stddef.h>
#include <stdint.h>

typedef struct pnet_EventError pnet_EventError;
typedef struct pnet_EventErrorSend pnet_EventErrorSend;
typedef struct pnet_EventMessage pnet_EventMessage;

typedef union pnet_Event pnet_Event;

/**
 * Peer-to-peer network event type.
 */
typedef enum pnet_EventType {
    PNET_EVENT_ERROR,
    PNET_EVENT_MESSAGE,
} pnet_EventType;

/**
 * An inbound peer-to-peer network message.
 */
struct pnet_EventMessage {
    /// Type of message.
    pnet_EventType type;

     /// Message nonce, useful for message or event association.
    kint_t nonce;

    /// The sender of the message.
    pnet_Host sender;

    /// Arbitrary data type identifier.
    uint16_t tag;

    /// Message payload data.
    mem_t data;
};

/**
 * A general network error.
 */
struct pnet_EventError {
    /// Type of message.
    pnet_EventType type;

    /// Message nonce, if any.
    kint_t nonce;

    /// Receiver or sender of failing message.
    pnet_Host host;

    /// Message tag, if any.
    uint16_t tag;

    /// Identifies type of error.
    err_t code;
};

/**
 * A inbound peer-to-peer network event.
 */
union pnet_Event {
    /**
     * Type of message.
     *
     * Determines what event variant is legal to access, and is always legal to
     * access itself.
     */
    pnet_EventType as_type;

    /**
     * Error event.
     *
     * @note Access legal if `type == PNET_EVENT_ERROR`.
     */
    pnet_EventError as_error;

    /**
     * Message vent.
     *
     * @note Access legal if `type == PNET_EVENT_MESSAGE`.
     */
    pnet_EventMessage as_message;
};

#endif