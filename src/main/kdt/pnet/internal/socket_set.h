#ifndef KDT_PNET_INTERNAL_SOCKET_SET_H
#define KDT_PNET_INTERNAL_SOCKET_SET_H

#include <stdbool.h>

#ifdef KDT_USE_POSIX
#include <sys/select.h>
#endif

typedef struct _pnet_Socket _pnet_Socket;
typedef struct _pnet_SocketSet _pnet_SocketSet;

struct _pnet_SocketSet {
#ifdef KDT_USE_POSIX
    int fd_server;
    int fd_count;
    fd_set fd_set;
    int fd_max;
#endif
};

void _pnet_ClearSocket(_pnet_SocketSet *set, _pnet_Socket *socket);
bool _pnet_IsSocketReady(_pnet_SocketSet *set, _pnet_Socket *socket);
void _pnet_ForEachReadySocket(_pnet_SocketSet *set, bool *accept, void *data,
                              bool (*callback)(void *, _pnet_Socket *));

#endif
