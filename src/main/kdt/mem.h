/**
 * Various utilities for managing fixed-size blocks of memory.
 *
 * @file
 */
#ifndef KDT_MEM_H
#define KDT_MEM_H

#include <kdt/err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct mem_t mem_t;

/**
 * A cursor for iterating over a region of arbitrary memory.
 */
struct mem_t {
    /// Pointer to first byte in memory.
    uint8_t *begin;

    /// Pointer to byte between begin and end.
    uint8_t *offset;

    /// Pointer to first byte immediately after memory.
    const uint8_t *end;
};

/**
 * Creates memory cursor for given buffer of size bytes.
 *
 * @param buffer Pointer to beginning of arbitrary memory region.
 * @param size Size of memory region, in bytes.
 * @return Memory cursor.
 */
mem_t mem_FromBuffer(uint8_t *buffer, size_t size);

/**
 * Creates memory cursor for given NULL-terminated C string.
 *
 * @param string Pointer to NULL-terminated string.
 * @return Memory cursor.
 */
mem_t mem_FromString(char *string);

/**
 * Gets total size of memory region, in bytes.
 *
 * @param mem Cursor.
 * @return Capacity, in bytes.
 */
size_t mem_Capacity(mem_t *mem);

/**
 * Reads `size` bytes to `out` from current cursor position.
 *
 * @param mem Memory to read from.
 * @param size Number of bytes to read from `mem`.
 * @param out Pointer to memory to receive read bytes.
 * @return Number of bytes read.
 */
size_t mem_Read(mem_t *mem, size_t size, uint8_t *out);

/**
 * Reads big endian half-word at cursor.
 *
 * If the end of `mem` has been reached, the function returns 0.
 *
 * @param mem Memory to read from.
 * @param success Pointer to value set to indicate if reading succeeded, or NULL.
 * @return Read half-word, or 0.
 */
bool mem_ReadU16BE(mem_t *mem, uint16_t *out);

/**
 * Reads big endian word at cursor.
 *
 * If the end of `mem` has been reached, the function returns 0.
 *
 * @param mem Memory to read from.
 * @param success Pointer to value set to indicate if reading succeeded, or NULL.
 * @return Read half-word, or 0.
 */
bool mem_ReadU32BE(mem_t *mem, uint32_t *out);

/**
 * Rewinds cursor to beginning of memory region.
 *
 * @param mem Cursor to rewind.
 */
void mem_Reset(mem_t *mem);

/**
 * Gets current size of memory region, in bytes.
 *
 * @param mem Cursor.
 * @return Space used, in bytes.
 */
size_t mem_Size(mem_t *mem);

/**
 * Advances memory cursor size bytes.
 *
 * If the operation fails, the memory cursor is left unchanged.
 *
 * @param mem Cursor to advance.
 * @param size Number of bytes to skip.
 * @return Whether cursor could be advanced size bytes.
 */
bool mem_Skip(mem_t *mem, size_t size);

/**
 * Skips given `character`, if at current memory offset.
 *
 * @param mem Cursor.
 * @param character Character to skip.
 * @return Whether or not `character` was skipped.
 */
bool mem_SkipCharacter(mem_t *mem, char character);

/**
 * Skips any ASCII whitespace characters.
 *
 * @param mem Cursor.
 */
void mem_SkipSpace(mem_t *mem);

/**
 * Gets space left before the end of the memory region is reached, in bytes.
 *
 * @param mem Cursor.
 * @return Space left, in bytes.
 */
size_t mem_Space(mem_t *mem);

/**
 * Writes buffer to cursor.
 *
 * If the buffer does not fit in cursor memory, the operation fails and `mem`
 * is left untouched.
 *
 * @param mem Cursor to write to.
 * @param buffer Pointer to beginning of memory region to write.
 * @param size Length of memory region to write.
 * @return Number of bytes written to `mem`.
 */
size_t mem_Write(mem_t *mem, void *buffer, size_t size);

/**
 * Writes byte to cursor.
 *
 * @param mem Cursor to write to.
 * @param byte Byte to write.
 * @return Whether there was space left to write the byte.
 */
bool mem_Write8(mem_t *mem, uint8_t byte);

/**
 * Writes big endian half-word to cursor.
 *
 * @param mem Cursor to write to.
 * @param byte Byte to write.
 * @return Whether there was space left to write the byte.
 */
bool mem_WriteU16BE(mem_t *mem, uint16_t half);

/**
 * Writes big endian word to cursor.
 *
 * @param mem Cursor to write to.
 * @param byte Byte to write.
 * @return Whether there was space left to write the byte.
 */
bool mem_WriteU32BE(mem_t *mem, uint32_t half);

/**
 * Writes formatted string to memory.
 *
 * Uses `vsnprintf` internally, which means that the flag characters it
 * provides also are supported by this function. If the formatted string does
 * not fit in cursor memory, it is silently truncated.
 *
 * @param mem Cursor to write to.
 * @param format Format string.
 * @param ... Format arguments, if any.
 * @return ERR_NONE only if operation succeeded.
 */
err_t mem_WriteF(mem_t *mem, const char *format, ...);

/**
 * Writes buffer to cursor, in hexadecimal ASCII form.
 *
 * If the hexadecimal form of the buffer does not fit in cursor memory, it is
 * silently truncated. If there is room available, a terminating NULL byte is
 * written to the end of the cursor, without advancing it. Neither the memory
 * cursor or the buffer may point to memory regions that overlap.
 *
 * @param mem Cursor to write to.
 * @param buffer Pointer to beginning of memory region to write.
 * @param size Length of memory region to write.
 */
void mem_WriteX(mem_t *mem, const uint8_t *buffer, size_t size);

#endif