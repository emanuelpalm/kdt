#include "socket_set.h"
#include "socket.h"

#ifdef KDT_USE_POSIX

inline
void _pnet_ClearSocket(_pnet_SocketSet *set, _pnet_Socket *socket) {
    FD_CLR(socket->fd, &set->fd_set);
    set->fd_count -= 1;
}

inline
bool _pnet_IsSocketReady(_pnet_SocketSet *set, _pnet_Socket *socket) {
    return FD_ISSET(socket->fd, &set->fd_set);
}

inline
void _pnet_ForEachReadySocket(_pnet_SocketSet *set, bool *accept, void *data,
                              bool (*callback)(void *, _pnet_Socket *)) {
    *accept = false;
    for (int fd = 0; fd <= set->fd_max && set->fd_count > 0; ++fd) {
        if (FD_ISSET(fd, &set->fd_set)) {
            continue;
        }
        set->fd_count -= 1;
        if (fd == set->fd_server) {
            *accept = true;
            continue;
        }
        if (!callback(data, &(_pnet_Socket) {.fd = fd})) {
            break;
        }
    }
}

#else
#error No supported internal PNET socket set implementation.
#endif

