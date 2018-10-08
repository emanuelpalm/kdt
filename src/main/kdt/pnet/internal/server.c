#include "server.h"
#include "socket.h"
#include "socket_set.h"

#ifdef KDT_USE_POSIX
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>

union _sockaddr_any {
    struct sockaddr_in ipv4;
    struct sockaddr_in6 ipv6;
};

inline
err_t _pnet_Open(_pnet_Server *server, pnet_Host *interface, _pnet_OnError on_error) {
    assert(server != NULL);
    assert(interface != NULL);

    // Set `interface` defaults, unless already set.
    if (interface->internet == PNET_INTERNET_NONE) {
        interface->internet = PNET_INTERNET_IPV6;
    }
    if (interface->transport == PNET_TRANSPORT_NONE) {
        interface->transport = PNET_TRANSPORT_TCP;
    }

    // socket() arguments.
    int domain, type;

    // bind() arguments.
    union _sockaddr_any sockaddr = {0};
    socklen_t socklen;

    // Details about data to save to `transport`.
    void *addr;
    size_t addrlen;
    uint16_t *port;

    // Determine socket configuration.
    {
        switch (interface->internet) {
        case PNET_INTERNET_IPV4:
            domain = AF_INET;

            sockaddr.ipv4.sin_family = AF_INET;
            sockaddr.ipv4.sin_addr.s_addr = ((in_addr_t *) interface->address)[0];
            sockaddr.ipv4.sin_port = htons(interface->port);
            socklen = sizeof(struct sockaddr_in);

            addr = &sockaddr.ipv4.sin_addr;
            addrlen = 4;
            port = &sockaddr.ipv4.sin_port;
            break;

        case PNET_INTERNET_IPV6:
            domain = AF_INET6;

            sockaddr.ipv6.sin6_family = AF_INET6;
            memcpy(&sockaddr.ipv6.sin6_addr, interface->address, 16);
            sockaddr.ipv6.sin6_port = htons(interface->port);
            socklen = sizeof(struct sockaddr_in6);

            addr = &sockaddr.ipv6.sin6_addr;
            addrlen = 16;
            port = &sockaddr.ipv6.sin6_port;
            break;

        default:
            return EINVAL;
        }
        switch (interface->transport) {
        case PNET_TRANSPORT_TCP:
            type = SOCK_STREAM;
            break;

        default:
            return EINVAL;
        }
    }

    err_t err = ERR_NONE;

    // Setup socket.
    int fd;
    {
        const int on = 1;
        int flags;

        if ((fd = socket(domain, type, 0)) < 0) {
            err = errno;
            goto leave;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int)) != 0) {
            err = errno;
            goto leave_close;
        }
        if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
            err = errno;
            goto leave_close;
        }
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
            err = errno;
            goto leave_close;
        }
        if (bind(fd, (struct sockaddr *) &sockaddr, socklen) != 0) {
            err = errno;
            goto leave_close;
        }
        if (getsockname(fd, (struct sockaddr *) &sockaddr, &socklen) != 0) {
            err = errno;
            goto leave_close;
        }
        if (listen(fd, KDT_N_BACKLOG) != 0) {
            err = errno;
            goto leave_close;
        }
    }

    // Initialize server fields.
    {
        server->fd = fd;
        server->fd_max = fd;
        FD_ZERO(&server->fd_set);
        FD_SET(fd, &server->fd_set);

        server->interface = interface;
    }

    // Update `interface` data, again.
    {
        if (addr != NULL) {
            memcpy(interface->address, addr, addrlen);
        }
        if (port != NULL) {
            interface->port = *port;
        }
    }

    // Set error handler.
    server->on_error = on_error;

    goto leave;

leave_close:
    close(fd);

leave:
    return err;
}

inline
void _pnet_Close(_pnet_Server *server) {
    for (int i = 0; i <= server->fd_max; ++i) {
        if (FD_ISSET(i, &server->fd_set)) {
            close(i);
        }
    }
}

inline
err_t _pnet_Accept(_pnet_Server *server) {
    int fd;
    do {
        if ((fd = accept(server->fd, NULL, NULL)) < 0) {
            return errno;
        }
        FD_SET(fd, &server->fd_set);
        if (server->fd_max < fd) {
            server->fd_max = fd;
        }
    } while (fd >= 0);
    return ERR_NONE;
}

inline
void _pnet_CloseSocket(_pnet_Server *server, _pnet_Socket *socket) {
    FD_CLR(socket->fd, &server->fd_set);
    if (server->fd_max == socket->fd) {
        server->fd_max -= 1;
    }
    close(socket->fd);
}

err_t _pnet_Connect(_pnet_Server *server, const pnet_Host *host, _pnet_Socket *out) {
    assert(server != NULL);
    assert(host != NULL);
    assert(out != NULL);

    if (host->internet != server->interface->internet) {
        return ERR_NOT_COMPATIBLE; // Should we handle multiple internet protocols at once?
    }
    if (host->transport != server->interface->transport) {
        return ERR_NOT_COMPATIBLE; // Should we handle multiple transport protocols at once?
    }

    // socket() arguments.
    int domain, type;

    // connect() arguments.
    union _sockaddr_any sockaddr = {0};
    socklen_t socklen;

    // Determine socket configuration.
    {
        switch (host->internet) {
        case PNET_INTERNET_IPV4:
            domain = AF_INET;

            sockaddr.ipv4.sin_family = AF_INET;
            sockaddr.ipv4.sin_addr.s_addr = ((in_addr_t *) host->address)[0];
            sockaddr.ipv4.sin_port = htons(host->port);
            socklen = sizeof(struct sockaddr_in);
            break;

        case PNET_INTERNET_IPV6:
            domain = AF_INET6;

            sockaddr.ipv6.sin6_family = AF_INET6;
            memcpy(&sockaddr.ipv6.sin6_addr, host->address, 16);
            sockaddr.ipv6.sin6_port = htons(host->port);
            socklen = sizeof(struct sockaddr_in6);
            break;

        default:
            return EINVAL;
        }
        switch (host->transport) {
        case PNET_TRANSPORT_TCP:
            type = SOCK_STREAM;
            break;

        default:
            return EINVAL;
        }
    }

    err_t err = ERR_NONE;

    // Setup socket and connect.
    int fd;
    {
        const int on = 1;
        int flags;

        if ((fd = socket(domain, type, 0)) < 0) {
            err = errno;
            goto leave;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int)) != 0) {
            err = errno;
            goto leave_close;
        }
        if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
            err = errno;
            goto leave_close;
        }
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
            err = errno;
            goto leave_close;
        }
        if (connect(fd, (struct sockaddr *) &sockaddr, socklen) != 0) {
            if (errno != EINPROGRESS) {
                err = errno;
                goto leave_close;
            }
        }
    }

    out->fd = fd;

    goto leave;

leave_close:
    close(fd);

leave:
    return err;
}

inline
void _pnet_HandleError(_pnet_Server *server, _pnet_Error *error) {
    assert(server != NULL);
    assert(error != NULL);

    server->on_error.callback(error, server->on_error.data);
}

err_t _pnet_PollReadableSockets(const _pnet_Server *server, _pnet_SocketSet *out) {
    out->fd_server = server->fd;
    out->fd_max = server->fd_max;
    memcpy(&out->fd_set, &server->fd_set, sizeof(fd_set));

    struct timeval timeout = {0};
    out->fd_count = select(out->fd_max + 1, &out->fd_set, NULL, NULL, &timeout);
    if (out->fd_count < 0) {
        return errno;
    }
    return ERR_NONE;
}

err_t _pnet_PollWritableSockets(const _pnet_Server *server, _pnet_SocketSet *out) {
    out->fd_server = server->fd;
    out->fd_max = server->fd_max;
    memcpy(&out->fd_set, &server->fd_set, sizeof(fd_set));

    struct timeval timeout = {0};
    out->fd_count = select(out->fd_max + 1, NULL, &out->fd_set, NULL, &timeout);
    if (out->fd_count < 0) {
        return errno;
    }
    return ERR_NONE;
}

err_t _pnet_ResolveHost(const _pnet_Server *server, const _pnet_Socket *socket,
                        pnet_Host *out) {
    err_t err;

    union _sockaddr_any sender;
    socklen_t size = sizeof(sender);
    err = getpeername(socket->fd, (struct sockaddr *) &sender, &size);
    if (err != ERR_NONE) {
        return err;
    }
    out->internet = server->interface->internet;
    out->transport = server->interface->transport;
    switch (out->internet) {
    case PNET_INTERNET_IPV4:
        out->port = ntohs(sender.ipv4.sin_port);
        memcpy(out->address, &sender.ipv4.sin_addr, sizeof(sender.ipv4.sin_addr));
        break;

    case PNET_INTERNET_IPV6:
        out->port = ntohs(sender.ipv6.sin6_port);
        memcpy(out->address, &sender.ipv6.sin6_addr, sizeof(sender.ipv6.sin6_addr));
        break;

    default:
        return EINVAL;
    }
    return ERR_NONE;
}

#else
#error No supported internal PNET server implementation.
#endif