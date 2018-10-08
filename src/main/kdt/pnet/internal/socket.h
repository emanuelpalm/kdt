#ifndef KDT_PNET_INTERNAL_SOCKET_H
#define KDT_PNET_INTERNAL_SOCKET_H

#include "../host.h"
#include <kdt/err.h>
#include <stddef.h>
#include <stdint.h>

typedef struct _pnet_Socket _pnet_Socket;

struct _pnet_Socket {
#ifdef KDT_USE_POSIX
    /// Socket file descriptor.
    int fd;
#endif
};

static const _pnet_Socket SOCKET_EMPTY =
#ifdef KDT_USE_POSIX
    {.fd = -1};
#else
{0};
#endif

bool _pnet_IsSocketEmpty(const _pnet_Socket *socket);
err_t _pnet_Send(_pnet_Socket *socket, size_t *size, uint8_t *data);
err_t _pnet_Receive(_pnet_Socket *socket, size_t *size, uint8_t *out);

#endif
