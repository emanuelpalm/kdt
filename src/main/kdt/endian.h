/**
 * Functions for converting machine words between different kinds of endianess.
 *
 * @file
 */
#ifndef KDT_ENDIAN_H
#define KDT_ENDIAN_H

#include <kdt/def.h>

#ifdef KDT_GCC5_BUILTINS

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define endian_IntoBE16(x) __builtin_bswap16 (x)
#define endian_IntoBE32(x) __builtin_bswap32 (x)
#define endian_IntoBE64(x) __builtin_bswap64 (x)

#define endian_IntoLE16(x) (x)
#define endian_IntoLE32(x) (x)
#define endian_IntoLE64(x) (x)

#define endian_FromBE16(x) __builtin_bswap16 (x)
#define endian_FromBE32(x) __builtin_bswap32 (x)
#define endian_FromBE64(x) __builtin_bswap64 (x)

#define endian_FromLE16(x) (x)
#define endian_FromLE32(x) (x)
#define endian_FromLE64(x) (x)
# else
#define endian_IntoBE16(x) (x)
#define endian_IntoBE32(x) (x)
#define endian_IntoBE64(x) (x)

#define endian_IntoLE16(x) __builtin_bswap16 (x)
#define endian_IntoLE32(x) __builtin_bswap_32 (x)
#define endian_IntoLE64(x) __builtin_bswap_64 (x)

#define endian_FromBE16(x) (x)
#define endian_FromBE32(x) (x)
#define endian_FromBE64(x) (x)

#define endian_FromLE16(x) __builtin_bswap16 (x)
#define endian_FromLE32(x) __builtin_bswap_32 (x)
#define endian_FromLE64(x) __builtin_bswap_64 (x)
#endif

#else
#error No supported endianess conversion implementations.
#endif

#endif
