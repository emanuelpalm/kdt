#include "host.h"
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef KDT_USE_POSIX
#include <arpa/inet.h>
#else
#warning No networking support.
#endif

#define _TRY(ERR) do {         \
    const err_t _err = (ERR); \
    if (_err != ERR_NONE) {   \
        return _err;          \
     }                        \
} while (0)

const char *pnet_GetInternetDescription(uint8_t internet) {
    const char *s;
    switch (internet) {
    case PNET_INTERNET_IPV4:
        s = "IPv4";
        break;
    case PNET_INTERNET_IPV6:
        s = "IPv6";
        break;
    default:
        s = NULL;
        break;
    }
    return s;
}

const char *pnet_GetTransportDescription(uint8_t transport) {
    const char *s;
    switch (transport) {
    case PNET_TRANSPORT_TCP:
        s = "TCP";
        break;
    default:
        s = NULL;
        break;
    }
    return s;
}

err_t pnet_ReadAddressText(mem_t *mem, uint8_t internet, uint8_t *out) {
    assert(mem != NULL);
    assert(out != NULL);

#ifdef KDT_USE_POSIX
    int af;
    char str[INET6_ADDRSTRLEN + 1];
    size_t str_size = 0;
    switch (internet) {
    case PNET_INTERNET_IPV4:
        af = AF_INET;
        while (mem->offset < mem->end && str_size < INET_ADDRSTRLEN) {
            char c = (char) mem->offset[str_size];
            if (!((c >= '0' && c <= '9') || c == '.')) {
                break;
            }
            str[str_size] = c;
            str_size += 1;
        }
        break;

    case PNET_INTERNET_IPV6:
        af = AF_INET6;
        while (mem->offset < mem->end && str_size < INET6_ADDRSTRLEN) {
            char c = (char) mem->offset[str_size];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
                  (c >= 'A' && c <= 'F') || c == ':')) {
                break;
            }
            str[str_size] = c;
            str_size += 1;
        }
        break;

    default:
        return EINVAL;
    }
    str[str_size] = '\0';

    switch (inet_pton(af, str, out)) {
    case 1:
        mem_Skip(mem, str_size);
        return ERR_NONE;
    case 0:
        return EINVAL;
    default:
        return errno;
    }
#endif
}

err_t pnet_ReadHostText(mem_t *mem, pnet_Host *out) {
    assert(mem != NULL);
    assert(out != NULL);

    err_t err;
    bool guess_internet = false;
    bool skipped_bracket;

    mem_SkipSpace(mem);
    err = pnet_ReadInternetText(mem, &out->internet);
    if (err != ERR_NONE) {
        guess_internet = true;
    }

    mem_SkipSpace(mem);
    if (mem_SkipCharacter(mem, '/')) {
        mem_SkipSpace(mem);
        _TRY(pnet_ReadTransportText(mem, &out->transport));
    }
    else {
        out->transport = PNET_TRANSPORT_TCP;
    }

    mem_SkipSpace(mem);
    skipped_bracket = mem_SkipCharacter(mem, '[');
    if (guess_internet) {
        out->internet = skipped_bracket
            ? PNET_INTERNET_IPV6
            : PNET_INTERNET_IPV4;
    }

    mem_SkipSpace(mem);
    _TRY(pnet_ReadAddressText(mem, out->internet, out->address));

    mem_SkipSpace(mem);
    mem_SkipCharacter(mem, ']');

    mem_SkipSpace(mem);
    if (mem_SkipCharacter(mem, ':')) {
        mem_SkipSpace(mem);
        _TRY(pnet_ReadPortText(mem, &out->port));
    } else {
        out->port = 0;
    }
    return ERR_NONE;
}

err_t pnet_ReadInternetText(mem_t *mem, uint8_t *out) {
    assert(mem != NULL);
    assert(out != NULL);

    if (strncasecmp((char *) mem->offset, "ipv4", 4) == 0) {
        *out = PNET_INTERNET_IPV4;
        mem_Skip(mem, 4);
        return ERR_NONE;
    }
    if (strncasecmp((char *) mem->offset, "ipv6", 4) == 0) {
        *out = PNET_INTERNET_IPV6;
        mem_Skip(mem, 4);
        return ERR_NONE;
    }
    if (strncasecmp((char *) mem->offset, "none", 4) == 0) {
        *out = PNET_INTERNET_NONE;
        mem_Skip(mem, 4);
        return ERR_NONE;
    }
    return ERR_NOT_VALID;
}

err_t pnet_ReadTransportText(mem_t *mem, uint8_t *out) {
    assert(mem != NULL);
    assert(out != NULL);

    if (strncasecmp((char *) mem->offset, "tcp", 3) == 0) {
        *out = PNET_TRANSPORT_TCP;
        mem_Skip(mem, 3);
        return ERR_NONE;
    }
    if (strncasecmp((char *) mem->offset, "none", 4) == 0) {
        *out = PNET_TRANSPORT_NONE;
        mem_Skip(mem, 4);
        return ERR_NONE;
    }
    return ERR_NOT_VALID;
}

err_t pnet_ReadPortText(mem_t *mem, uint16_t *out) {
    assert(mem != NULL);
    assert(out != NULL);

    errno = 0;

    char *end;
    unsigned long port = strtoul((const char *) mem->offset, &end, 10);
    if (errno != 0) {
        return errno;
    }
    if (port > UINT16_MAX) {
        return ERANGE;
    }
    if (((uint8_t *) end <= mem->end) && ((uint8_t *) end > mem->begin)) {
        mem->offset = (uint8_t *) end;
    }
    else {
        return EINVAL;
    }
    *out = (uint16_t) port;
    return ERR_NONE;
}

err_t pnet_WriteHostText(const pnet_Host *host, mem_t *mem) {
    assert(host != NULL);
    assert(mem != NULL);

    // Write internet and transport.
    int af;
    {
        const char *in, *tr;
        switch (host->internet) {
#ifdef KDT_USE_POSIX
        case PNET_INTERNET_IPV4:
            in = "IPv4";
            af = AF_INET;
            break;
        case PNET_INTERNET_IPV6:
            in = "IPv6";
            af = AF_INET6;
            break;
#endif
        default:
            in = "?";
            af = AF_UNSPEC;
            break;
        }
        tr = pnet_GetTransportDescription(host->transport);
        if (tr == NULL) {
            tr = "?";
        }
        _TRY(mem_WriteF(mem, "%s/%s ", in, tr));
    }

    // Write IP address.
    {
        if (host->internet == PNET_INTERNET_IPV6) {
            mem_Write8(mem, '[');
        }
#ifdef KDT_USE_POSIX
        const void *cp = host->address;
        char *buf = (char *) mem->offset;
        size_t size = mem_Space(mem);
        if (size > 255) {
            size = 255;
        }
        if (inet_ntop(af, cp, buf, (socklen_t) size) == NULL) {
            mem_Write8(mem, '?');
        }
        else {
            mem_Skip(mem, strlen(buf));
        }
#endif
        if (host->internet == PNET_INTERNET_IPV6) {
            mem_Write8(mem, ']');
        }
    }

    // Write port.
    _TRY(mem_WriteF(mem, ":%d", host->port));

    return ERR_NONE;
}