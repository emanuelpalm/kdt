#ifndef KDT_PNET_MESSAGE_H
#define KDT_PNET_MESSAGE_H

#include "host.h"
#include <kdt/kint.h>
#include <kdt/mem.h>
#include <stddef.h>
#include <stdint.h>

typedef struct pnet_Message pnet_Message;

/**
 * An outbound peer-to-peer network message.
 */
struct pnet_Message {
    /**
     * The intended receiver of the message.
     */
    pnet_Host receiver;

    /**
     * Message nonce, useful for keeping track of message or event associations.
     *
     * If an outbound message initiates a communication, the nonce should be
     * set to a secure random number. If, on the other hand, an outbound
     * message is a response to a previous request message, the nonce should be
     * a copy of the nonce in that message.
     */
    kint_t nonce;

    /**
     * Arbitrary data type identifier.
     *
     * Identifies the kind of information present in `data`, assuming the
     * message receiver knows how to identify the tag.
     */
    uint16_t tag;

    /**
     * Message payload data.
     */
    mem_t data;
};

#endif