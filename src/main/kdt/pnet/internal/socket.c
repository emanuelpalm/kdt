#include "socket.h"

#ifdef KDT_USE_POSIX
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>

inline
bool _pnet_IsSocketEmpty(const _pnet_Socket *socket) {
    return socket->fd == -1;
}

inline
err_t _pnet_Send(_pnet_Socket *socket, size_t *size, uint8_t *data) {
    assert(socket != NULL);
    assert(size != NULL);
    assert(data != NULL || *size == 0);

    ssize_t status = send(socket->fd, data, *size, 0);
    if (status < 0) {
        status = errno;

        // Ensure there is no problem with the connection itself.
        int err = 0;
        socklen_t err_size = sizeof(int);
        getsockopt(socket->fd, SOL_SOCKET, SO_ERROR, &err, &err_size);

        return err != 0 ? err : (err_t) status;
    }
    *size = (size_t) status;
    return ERR_NONE;
}

inline
err_t _pnet_Receive(_pnet_Socket *socket, size_t *size, uint8_t *out) {
    assert(socket != NULL);
    assert(size != NULL);
    assert(out != NULL || *size == 0);

    ssize_t status = recv(socket->fd, out, *size, 0);
    if (status < 0) {
        return errno;
    }
    *size = (size_t) status;
    return ERR_NONE;
}

#else
#error No supported internal PNET socket implementation.
#endif