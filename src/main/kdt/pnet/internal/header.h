#ifndef KDT_PNET_INTERNAL_HEADER_H
#define KDT_PNET_INTERNAL_HEADER_H

/**
 * The number of bytes preceding the payload of a transmitted or received
 * message.
 *
 * The first `sizeof(kint_t)` bytes is a nonce, the following 2 a tag, while
 * the last 2 are the size of the payload, immediately followed by the message
 * payload. Both the tag and size need to be written or read in network byte
 * order (big-endian).
 */
#define _PNET_HEADER_SIZE (sizeof(kint_t) + sizeof(uint16_t) + sizeof(uint16_t))

#endif
