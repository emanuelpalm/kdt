#ifndef KDT_KDM_INTERNAL_PROTOCOL_H
#define KDT_KDM_INTERNAL_PROTOCOL_H

#include "table.h"
#include <kdt/err.h>
#include <kdt/kint.h>

typedef struct _kdm_Protocol _kdm_Protocol;
typedef struct kvs_t kvs_t;
typedef struct pnet_t pnet_t;
typedef struct pnet_EventMessage pnet_EventMessage;
typedef struct pnet_Host pnet_Host;

struct _kdm_Protocol {
    /// Whether or not the Kademlia protocol handler is to be running.
    bool running;

    /// Local host identifier.
    kint_t id;

    /// Routing table.
    _kdm_Table table;

    /// Key/value store.
    kvs_t *store;

    /// Peer-to-peer networking node.
    pnet_t *pnet;
};

err_t _kdm_InitProtocol(_kdm_Protocol *protocol, kvs_t *store, pnet_t *pnet);
kint_t *_kdm_GetClientID(_kdm_Protocol *protocol);
err_t _kdm_Join(_kdm_Protocol *protocol, const pnet_Host *peer);
err_t _kdm_Poll(_kdm_Protocol *protocol);

#endif
