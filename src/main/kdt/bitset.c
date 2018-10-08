#include "bitset.h"
#include <assert.h>
#include <kdt/def.h>

inline
void bitset_Init(bitset_t *bitset, uint8_t *bytes, size_t size) {
    bitset->bytes = bytes;
    bitset->size = size;
    mtx_Init(&bitset->lock);
}

bool bitset_Allocate(bitset_t *bitset, size_t *index) {
    assert(bitset != NULL);
    assert(index != NULL);

    bool status = false;

    mtx_Lock(&bitset->lock);
    {
        const size_t size = bitset->size;

#ifdef KDT_GCC5_BUILTINS
        size_t i = 0;
        size_t bit;
        {
            unsigned long *words = (unsigned long *) bitset->bytes;
            const size_t end = size / sizeof(unsigned long);
            while (i + sizeof(unsigned long) <= end) {
                unsigned long word = words[i];
                if (word != 0) {
                    bit = __builtin_ffsl(word) - 1u;
                    words[i] &= ~(1u << bit);
                    *index = (i * 8u) + bit;
                    status = true;
                    goto leave;
                }
                i += sizeof(unsigned long);
            }
        }
        {
            uint8_t *bytes = bitset->bytes;
            while (i < size) {
                uint8_t byte = bytes[i];
                if (byte != 0) {
                    bit = __builtin_ffs(byte) - 1u;
                    bytes[i] &= ~(1u << bit);
                    *index = (i * 8u) + bit;
                    status = true;
                    goto leave;
                }
                i += 1;
            }
        }
#else
        uint8_t *bytes = bitset->bytes;
        for (size_t i = 0; i < size; ++i) {
            uint8_t byte = bytes[i];
            if (byte != 0) {
                size_t bit = 0u;
                if ((byte & 0x0Fu) == 0u) {
                    bit += 4u;
                    byte >>= 4u;
                }
                if ((byte & 0x03u) == 0u) {
                    bit += 2u;
                    byte >>= 2u;
                }
                if ((byte & 0x01u) == 0u) {
                    bit += 1u;
                }
                bytes[i] &= ~(1u << bit);
                *index = (i * 8u) + bit;
                status = true;
                goto leave;
            }
        }
#endif
    }
leave:
    mtx_Unlock(&bitset->lock);
    return status;
}

inline
void bitset_Clear(bitset_t *bitset, size_t index) {
    assert(bitset != NULL);
    assert(bitset->size > index / 8u);

    mtx_Lock(&bitset->lock);
    {
        bitset->bytes[index / 8u] &= ~(1u << (0x7u & index));
    }
    mtx_Unlock(&bitset->lock);
}

inline
void bitset_Set(bitset_t *bitset, size_t index) {
    assert(bitset != NULL);
    assert(bitset->size > index / 8u);

    mtx_Lock(&bitset->lock);
    {
        bitset->bytes[index / 8u] |= 1u << (0x7u & index);
    }
    mtx_Unlock(&bitset->lock);
}