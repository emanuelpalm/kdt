/**
 * An implementation of the Kademlia algorithm.
 *
 * @file
 */
#ifndef KDT_KDM_H
#define KDT_KDM_H

#include <kdt/pnet/host.h>
#include <kdt/err.h>

typedef struct kvs_t kvs_t;
typedef struct pnet_t pnet_t;

/**
 * Starts kademlia worker pool.
 *
 * @param store Pointer to key/value store to use for storing data.
 * @param pnet Pointer to PNET structure, used to handle message passing.
 * @param peer Network peer to use for joining Kademlia network.
 * @return ERR_NONE, only if operation was successful.
 */
err_t kdm_Start(kvs_t *store, pnet_t *pnet, const pnet_Host *peer);

/**
 * Blocks the calling thread until all Kademlia workers have been terminated.
 */
void kdm_Shutdown();

#endif
