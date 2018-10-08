#ifndef KDT_KDM_ID_H
#define KDT_KDM_ID_H

#include <kdt/def.h>
#include <kdt/mem.h>
#include <stddef.h>
#include <stdint.h>

#if (KDT_B8 % 2) != 0
#error KDT_B8 must be evenly dividable by 2.
#endif
#if (KDT_B8 % 4) != 0
#error KDT_B8 must be evenly dividable by 4.
#endif

typedef union kint_t kint_t;

/**
 * A Kademlia integer, used for identifiers, distances, prefixes, etc.
 *
 * The bits of a Kademlia integer may be accessed either as 8-bit, 16-bit or
 * 32-bit words. As Kademlia integers are to be strictly big-endian, care has
 * to be taken to preserve endianess when accessing it as arrays of integers
 * larger than 8 bits.
 */
union kint_t {
    uint8_t as_u8s[KDT_B8];
    uint16_t as_u16s[KDT_B8/2];
    uint32_t as_u32s[KDT_B8/4];
};

/**
 * Hashes given string of bytes, turning it into a Kademlia integer.
 *
 * @param bytes Pointer to bytes to hash.
 * @param size Number of bytes to hash.
 * @return New Kademlia integer.
 */
kint_t kint_Hash(const uint8_t *bytes, size_t size);

/**
 * Creates new random integer with a secure random generator available to the
 * platform.
 *
 * @return New random Kademlia integer.
 */
kint_t kint_Random();

/**
 * Counts leading zeroes in `x`.
 *
 * In other words, counts zeroes from the most significant bit and downwards.
 *
 * @param x Pointer to integer.
 * @return Number of leading zeroes.
 */
size_t kint_CLZ(const kint_t *x);

/**
 * Determines if `a` is lesser than, equal to or greater than `b`.
 *
 * @param a Pointer to integer.
 * @param b Pointer to integer.
 * @return `0` if `a == b`, `< 0` if `a < b`, and `> 0` if `a > b`.
 */
int kint_CMP(const kint_t *a, const kint_t *b);

/**
 * Determines whether or not given IDs are equal.
 *
 * @param a Pointer to integer.
 * @param b Pointer to integer.
 * @return Whether or not `a` and `b` are equal.
 */
bool kint_EQU(const kint_t *a, const kint_t *b);

/**
 * Writes textual representation of integer to memory region.
 *
 * @param x integer to format.
 * @param mem Memory region to write to.
 */
void kint_WriteText(const kint_t *x, mem_t *mem);

/**
 * Calculates binary XOR of a and b.
 *
 * @param a First compared Kademlia integer.
 * @param b Second compared Kademlia integer.
 * @return Absolute logical distance between a and b.
 */
kint_t kint_XOR(const kint_t *a, const kint_t *b);

#endif