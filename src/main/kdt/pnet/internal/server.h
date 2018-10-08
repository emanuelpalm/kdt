#ifndef KDT_PNET_INTERNAL_SERVER_H
#define KDT_PNET_INTERNAL_SERVER_H

#include "sender.h"
#include "receiver.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef KDT_USE_POSIX
#include <sys/select.h>
#endif

typedef struct _pnet_Error _pnet_Error;
typedef struct _pnet_OnError _pnet_OnError;
typedef struct _pnet_Server _pnet_Server;
typedef struct _pnet_Socket _pnet_Socket;
typedef struct _pnet_SocketSet _pnet_SocketSet;

struct _pnet_OnError {
    void (*callback)(_pnet_Error *, void *);
    void *data;
};

struct _pnet_Server {
#ifdef KDT_USE_POSIX
    /// Listener socket file descriptor.
    int fd;

    /// Number of highest active socket.
    int fd_max;

    /// Set of all client sockets.
    fd_set fd_set;
#endif

    /// Socket interface.
    const pnet_Host *interface;

    /// Function used for reporting errors.
    _pnet_OnError on_error;
};

struct _pnet_Error {
    kint_t *nonce;
    pnet_Host *host;
    uint16_t tag;
    err_t err;
};

err_t _pnet_Open(_pnet_Server *server, pnet_Host *interface, _pnet_OnError on_error);
void _pnet_Close(_pnet_Server *server);
err_t _pnet_Accept(_pnet_Server *server);
void _pnet_CloseSocket(_pnet_Server *server, _pnet_Socket *socket);
err_t _pnet_Connect(_pnet_Server *server, const pnet_Host *host, _pnet_Socket *out);
void _pnet_HandleError(_pnet_Server *server, _pnet_Error *error);
err_t _pnet_PollReadableSockets(const _pnet_Server *server, _pnet_SocketSet *out);
err_t _pnet_PollWritableSockets(const _pnet_Server *server, _pnet_SocketSet *out);
err_t _pnet_ResolveHost(const _pnet_Server *server, const _pnet_Socket *socket,
                        pnet_Host *out);

#endif
