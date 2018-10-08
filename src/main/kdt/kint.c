#include "kint.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef KDT_USE_OPENSSL
#include <openssl/rand.h>
#include <openssl/sha.h>
#if KDT_B8 == SHA512_DIGEST_LENGTH
#define KDT_HASH(D, N, MD) SHA512((D), (N), (unsigned char *) (MD))
#elif KDT_B8 == SHA384_DIGEST_LENGTH
#define KDT_HASH(D, N, MD) SHA384((D), (N), (unsigned char *) (MD))
#elif KDT_B8 == SHA256_DIGEST_LENGTH
#define KDT_HASH(D, N, MD) SHA256((D), (N), (unsigned char *) (MD))
#elif KDT_B8 == SHA224_DIGEST_LENGTH
#define KDT_HASH(D, N, MD) SHA224((D), (N), (unsigned char *) (MD))
#elif KDT_B8 == SHA_DIGEST_LENGTH
#define KDT_HASH(D, N, MD) SHA1((D), (N), (unsigned char *) (MD))
#else
#include <openssl/md5.h>
#include <stdio.h>
#if KDT_B8 == MD5_DIGEST_LENGTH
#define KDT_HASH(D, N, MD) MD5((D), (N) / sizeof(unsigned char), (unsigned char *) (MD))
#else
#error KDT_B8 does not match the output length of any known hashing algorithm.
#endif
#endif
#endif

inline
kint_t kint_Hash(const uint8_t *bytes, size_t size) {
    assert(bytes != NULL || size == 0);

    kint_t x = {0};
    KDT_HASH(bytes, size, &x.as_u8s);
    return x;
}

inline
kint_t kint_Random() {
    kint_t x = {0};
    const size_t size = sizeof(kint_t) / sizeof(unsigned char);
    if (!RAND_bytes((unsigned char *) &x, size)) {
        if (!RAND_poll() || !RAND_bytes((unsigned char *) &x, size)) {
            fputs("OpenSSL: No secure random available.", stderr);
            exit(EXIT_FAILURE);
        }
    }
    return x;
}

inline
size_t kint_CLZ(const kint_t *x) {
    for (size_t i = KDT_B8; i-- != 0;) {
        size_t byte = x->as_u8s[i];
        if (byte != 0) {
            size_t bits = ((KDT_B8 - 1 - i) * 8);
#ifdef KDT_GCC5_BUILTINS
            byte <<= ((sizeof(unsigned int) - 1) * 8);
            bits += ((size_t) __builtin_clz(byte));
#else
            if ((byte & 0xF0) == 0) {
                bits += 4;
                byte <<= 4;
            }
            if ((byte & 0xC0) == 0) {
                bits += 2;
                byte <<= 2;
            }
            if ((byte & 0x80) == 0) {
                bits += 1;
            }
#endif
            return bits;
        }
    }
    return KDT_B;
}

inline
int kint_CMP(const kint_t *a, const kint_t *b) {
    assert(a != NULL);
    assert(b != NULL);

#if KDT_ENDIANESS == KDT_ENDIANESS_BIG
    return memcmp(a, b, sizeof(kind_t));
#else
    if (a == b) {
        return 0;
    }
#if KDT_ENDIANESS == KDT_ENDIANESS_LITTLE && KDT_SIZEOF_INT > 2
    for (size_t i = KDT_B8 / 2; i-- != 0;) {
        const int d = a->as_u16s[i] - b->as_u16s[i];
        if (d != 0) {
            return d;
        }
    }
#else
    for (size_t i = KDT_B8; i-- != 0;) {
        const int d = a->u8[i] - b->u8[i];
        if (d != 0) {
            return d;
        }
    }
#endif
#endif
    return 0;
}

inline
bool kint_EQU(const kint_t *a, const kint_t *b) {
    assert(a != NULL);
    assert(b != NULL);

    return memcmp(a, b, sizeof(kint_t)) == 0;
}

inline
void kint_WriteText(const kint_t *x, mem_t *mem) {
    if (x == NULL) {
        mem_Write(mem, "{NULL}", 6);
        return;
    }
    mem_WriteX(mem, x->as_u8s, KDT_B8);
}

inline
kint_t kint_XOR(const kint_t *a, const kint_t *b) {
    assert(a != NULL);
    assert(b != NULL);

    kint_t id = {0};
    if (a == b) {
        return id;
    }
    for (size_t i = KDT_B8 / 4; i-- != 0;) {
        id.as_u32s[i] = a->as_u32s[i] ^ b->as_u32s[i];
    }
    return id;
}