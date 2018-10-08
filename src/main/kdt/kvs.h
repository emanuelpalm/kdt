/**
 * Various utilities for managing key/value stores.
 *
 * @file
 */
#ifndef KDT_KVS_H
#define KDT_KVS_H

#include <kdt/err.h>
#include <kdt/kint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef KDT_USE_LMDB
#include <lmdb.h>
#endif

typedef struct kvs_t kvs_t;

/**
 * A key/value store, maintaining entries with keys of KDT_B bits and values of
 * arbitrary size.
 */
struct kvs_t {
#ifdef KDT_USE_LMDB
    MDB_env *env;
    MDB_dbi dbi;
#endif
};

/**
 * Opens key/value store, allowing it to be used.
 *
 * If `dir` refers to a directory that does not exist, an attempt will be made
 * to create it. If, however, the parent directory also is missing, the attempt
 * will fail.
 *
 * @note Not thread-safe.
 *
 * @param directory Path to store root directory.
 * @param out Pointer to uninitialized store data structure.
 * @return ERR_NONE only if the operation was successful.
 */
err_t kvs_Open(const char *directory, kvs_t *out);

/**
 * Closes key/value store.
 *
 * After this function returns, the store pointer at is to be regarded as being
 * uninitialized.
 *
 * @note Not thread-safe.
 *
 * @param store Pointer to store.
 */
void kvs_Close(kvs_t *store);

#ifdef KDT_TEST
/**
 * Destroys entire store, deleting all of its contents.
 *
 * @note Given store must be opened, and does not need to be closed after the
 * function returns, even if it would fail. The function may assume that no
 * other processes are using the store.
 *
 * @note Not thread-safe.
 *
 * @param store Pointer to store.
 * @return ERR_NONE only if the operation was successful.
 */
err_t kvs_Drop(kvs_t *store);
#endif

/**
 * Attempts to delete value with `key`.
 *
 * Returns ERR_NOT_FOUND if no entry exists with given key.
 *
 * @note Thread-safe.
 *
 * @param store Pointer to store.
 * @param key Pointer to key.
 * @return ERR_NONE only if operation succeeded.
 */
err_t kvs_Delete(kvs_t *store, const kint_t *key);

/**
 * Attempts to copy value `key` of no more than `size` bytes to `out`.
 *
 * Returns ERR_NOT_FOUND if no entry exists with given key, or ERR_TOO_LARGE if
 * requested value does not fit in `out`.
 *
 * @note Thread-safe.
 *
 * @param store Pointer to store.
 * @param key Pointer to key.
 * @param out Pointer to mutated value.
 * @return ERR_NONE only if operation succeeded.
 */
err_t kvs_Get(kvs_t *store, const kint_t *key, mem_t *out);

/**
 * Provides string representation of error code, if given code is specific to
 * the current KVS implementation.
 *
 * The function is only guaranteed to return a non-NULL pointer if the error
 * code is larger than or equal to KVS_ERR_FIRST and smaller than or equal to
 * KVS_ERR_LAST.
 *
 * @note Most scenarios are likely better served by using `err_GetDescription()`
 * rather than this function, as that function is able to describe many more
 * error codes.
 *
 * @note Thread-safe.
 *
 * @param err Error to represent.
 * @return String representation, or NULL if unknown.
 */
const char *kvs_GetImplErrDescription(err_t err);

/**
 * @note Thread-safe.
 *
 * @return Whether given error code is related to this KVS implementation.
 */
bool kvs_IsImplErr(err_t err);

/**
 * Attempts to set value of `key`.
 *
 * Returns ERR_TOO_LARGE if the provided value is larger than
 * KVT_VALUE_SIZE_MAX bytes, or ERR_TOO_SMALL if the size of the provided value
 * is zero.
 *
 * @note Thread-safe.
 *
 * @param store Pointer to store.
 * @param key Pointer to key.
 * @param size Value size, in bytes. 0 is treated as 65536.
 * @param data Pointer to beginning of value data.
 * @return ERR_NONE only if operation succeeded.
 */
err_t kvs_Set(kvs_t *store, const kint_t *key, size_t size, uint8_t *data);

#endif
