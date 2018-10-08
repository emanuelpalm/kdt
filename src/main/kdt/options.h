/**
 * Handling of KDT command line options.
 *
 * @file
 */
#ifndef KDT_OPTIONS_H
#define KDT_OPTIONS_H

#include <kdt/pnet/host.h>

typedef struct options_t options_t;

/**
 * Application options.
 */
struct options_t {
    /// Local network interface.
    pnet_Host interface;

    /// Peer host to use for connecting to Kademlia network.
    pnet_Host peer;
};

/**
 * Loads application options form program environment.
 *
 * @param out Pointer to options structure.
 * @return ERR_NONE only if operation succeeded.
 */
err_t options_FromEnvironment(options_t *out);

#endif
