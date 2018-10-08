#ifndef KDT_PNET_HOST_H
#define KDT_PNET_HOST_H

#include <kdt/err.h>
#include <kdt/mem.h>
#include <stdbool.h>

#define PNET_ADDRESS_SIZE 16

typedef struct pnet_Host pnet_Host;

/**
 * Supported internet layer protocols.
 */
enum {
    PNET_INTERNET_NONE = (uint8_t) 0,
    PNET_INTERNET_IPV4,
    PNET_INTERNET_IPV6,
};

/**
 * Supported transport layer protocols.
 */
enum {
    PNET_TRANSPORT_NONE = (uint8_t) 0,
    PNET_TRANSPORT_TCP,
};

/**
 * A network host.
 */
struct pnet_Host {
    /// Internet layer protocol used to communicate with host.
    uint8_t internet;

    /// Transport layer protocol used to communicate with host.
    uint8_t transport;

    /// Host address.
    uint8_t address[PNET_ADDRESS_SIZE];

    /// Host port.
    uint16_t port;
};

/**
 * Returns textual representation of internet protocol identifier.
 *
 * @param internet Target internet protocol.
 * @return Text, or NULL if given protocol identifier is invalid.
 */
const char *pnet_GetInternetDescription(uint8_t internet);

/**
 * Returns textual representation of transport protocol identifier.
 *
 * @param transport Target transport protocol.
 * @return Text, or NULL if given protocol identifier is invalid.
 */
const char *pnet_GetTransportDescription(uint8_t transport);

/**
 * Reads textual representation of address to `out`.
 *
 * @note `out` must point to a memory region of at least PNET_ADDRESS_SIZE
 * bytes.
 *
 * @param mem Memory to read from.
 * @param internet Expected address internet protocol.
 * @param out Pointer to host structure.
 * @return ERR_NONE only if operation succeeded.
 */
err_t pnet_ReadAddressText(mem_t *mem, uint8_t internet, uint8_t* out);

/**
 * Reads textual representation of host to `out`.
 *
 * @param mem Memory to read from.
 * @param out Pointer to host structure.
 * @return ERR_NONE only if operation succeeded.
 */
err_t pnet_ReadHostText(mem_t *mem, pnet_Host *out);

/**
 * Reads textual representation of internet protocol to `out`.
 *
 * @param mem Memory to read from.
 * @param out Pointer to unsigned 8-bit integer.
 * @return ERR_NONE only if operation succeeded.
 */
err_t pnet_ReadInternetText(mem_t *mem, uint8_t *out);

/**
 * Reads textual representation of transport protocol to `out`.
 *
 * @param mem Memory to read from.
 * @param out Pointer to unsigned 8-bit integer.
 * @return ERR_NONE only if operation succeeded.
 */
err_t pnet_ReadTransportText(mem_t *mem, uint8_t *out);

/**
 * Reads textual representation of port number to `out`.
 *
 * @param mem Memory to read from.
 * @param out Pointer to unsigned 8-bit integer.
 * @return ERR_NONE only if operation succeeded.
 */
err_t pnet_ReadPortText(mem_t *mem, uint16_t *out);

/**
 * Writes textual representation of host to memory region.
 *
 * @param host Host to write.
 * @param mem Memory region to write to.
 * @return ERR_NONE only if operation succeeded.
 */
err_t pnet_WriteHostText(const pnet_Host *host, mem_t *mem);

#endif