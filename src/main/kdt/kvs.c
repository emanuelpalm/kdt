#include "kvs.h"

#ifdef KDT_USE_LMDB
#include <assert.h>
#include <limits.h>

#ifdef KDT_USE_POSIX
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#endif

err_t kvs_Open(const char *directory, kvs_t *out) {
    assert(directory != NULL);
    assert(out != NULL);

    memset(out, 0, sizeof(kvs_t));

#ifdef KDT_USE_POSIX
    // Ensure `directory` exists and is a directory.
    struct stat s;
    if (stat(directory, &s) != 0) {
        if (errno != ENOENT) {
            return errno;
        }
        if (mkdir(directory, 0770) != 0) {
            return errno;
        }
    }
    else if (!S_ISDIR(s.st_mode)) {
        return ENOTDIR;
    }
#endif

    // Determine size of LMDB mapping.
    size_t mapsize;
    const char *mapsize_str = getenv("KDT_LMDB_MAPSIZE");
    if (mapsize_str != NULL) {
        if ((mapsize = (size_t) strtoull(mapsize_str, NULL, 10)) == ULLONG_MAX) {
            return EINVAL;
        }
    }
    else {
#ifdef KDT_USE_POSIX
        struct statvfs v;
        if (statvfs(directory, &v) != 0) {
            return errno;
        }
        mapsize = (v.f_bsize * v.f_bavail) & ~0xFFFUL;
#else
        mapsize = 64 * 1024 * 1024;
#endif
    }

    int code;

    // Open and configure LMDB.
    if ((code = mdb_env_create(&out->env)) != MDB_SUCCESS) {
        goto leave;
    }
    if ((code = mdb_env_open(out->env, directory, 0, 0660)) != MDB_SUCCESS) {
        switch (code) {
        case MDB_VERSION_MISMATCH:
            code = ERR_NOT_COMPATIBLE;
            break;

        case MDB_INVALID:
            code = ERR_NOT_VALID;
            break;

        default:
            break;
        }
        goto leave_close_db_env;
    }
    int set_mapsize_retries_left = 4;
retry_set_mapsize:
    if ((code = mdb_env_set_mapsize(out->env, mapsize)) != MDB_SUCCESS) {
        switch (code) {
        case EINVAL:
            if (set_mapsize_retries_left-- > 0) {
                mapsize /= 2;
                goto retry_set_mapsize;
            }
            break;

        default:
            break;
        }
        goto leave_close_db_env;
    }
    MDB_txn *txn;
    if ((code = mdb_txn_begin(out->env, NULL, 0, &txn)) != MDB_SUCCESS) {
        if (code == MDB_PANIC) {
            code = ERR_PANIC;
        }
        goto leave_close_db_env;
    }
    if ((code = mdb_dbi_open(txn, NULL, 0, &out->dbi)) != MDB_SUCCESS) {
        if (code == MDB_NOTFOUND) {
            code = ERR_NOT_FOUND;
        }
        goto leave_abort_txn;
    }
    if ((code = mdb_txn_commit(txn)) != MDB_SUCCESS) {
        goto leave_abort_txn;
    }
    goto leave;

leave_abort_txn:
    mdb_txn_abort(txn);
leave_close_db_env:
    mdb_env_close(out->env);
leave:
    return code;
}

inline
void kvs_Close(kvs_t *store) {
    assert(store != NULL);

    mdb_dbi_close(store->env, store->dbi);
    mdb_env_close(store->env);
}

#ifdef KDT_TEST
err_t kvs_Drop(kvs_t *store) {
    assert(store != NULL);

#ifdef KDT_USE_POSIX
    const char *path;
    mdb_env_get_path(store->env, &path);
    const int fd = open(path, O_DIRECTORY);
    DIR *dirp = fdopendir(fd);
#endif

    int code;
    MDB_txn *txn;
    if ((code = mdb_txn_begin(store->env, NULL, 0, &txn)) != MDB_SUCCESS) {
        if (code == MDB_PANIC) {
            code = ERR_PANIC;
        }
        goto leave;
    }
    code = mdb_drop(txn, store->dbi, 1);
    mdb_txn_commit(txn);
leave:
    mdb_env_close(store->env);

#ifdef KDT_USE_POSIX
    errno = 0;
    while (true) {
        struct dirent *dir = readdir(dirp);
        if (dir == NULL) {
            if (errno != 0) {
                code = errno;
            }
            break;
        }
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }
        if (unlinkat(fd, dir->d_name, 0) != 0) {
            code = errno;
            break;
        }
    }
    closedir(dirp);
    close(fd);
#endif

    return code;
}
#endif

err_t kvs_Delete(kvs_t *store, const kint_t *key) {
    assert(store != NULL);
    assert(key != NULL);

    int code;
    MDB_txn *txn;
    if ((code = mdb_txn_begin(store->env, NULL, 0, &txn)) != MDB_SUCCESS) {
        if (code == MDB_PANIC) {
            code = ERR_PANIC;
        }
        goto leave;
    }
    MDB_val k = {.mv_size = KDT_B8, .mv_data = (void *) key};
    if ((code = mdb_del(txn, store->dbi, &k, NULL)) != MDB_SUCCESS) {
        goto leave_abort_txn;
    }
    code = mdb_txn_commit(txn);
    goto leave;

leave_abort_txn:
    mdb_txn_abort(txn);
leave:
    return code;
}

err_t kvs_Get(kvs_t *store, const kint_t *key, mem_t *out) {
    assert(store != NULL);
    assert(key != NULL);
    assert(out != NULL);

    int code;
    MDB_txn *txn;
    if ((code = mdb_txn_begin(store->env, NULL, MDB_RDONLY, &txn)) != MDB_SUCCESS) {
        if (code == MDB_PANIC) {
            code = ERR_PANIC;
        }
        goto leave;
    }
    MDB_val k = {.mv_size = KDT_B8, .mv_data = (void *) key};
    MDB_val v;
    if ((code = mdb_get(txn, store->dbi, &k, &v)) != MDB_SUCCESS) {
        if (code == MDB_NOTFOUND) {
            code = ERR_NOT_FOUND;
        }
        goto leave_abort_txn;
    }
    if (v.mv_size > mem_Space(out)) {
        mdb_txn_commit(txn);
        return ERR_NOT_FOUND;
    }
    mem_Write(out, v.mv_data, v.mv_size);
    mdb_txn_commit(txn);
    goto leave;

leave_abort_txn:
    mdb_txn_abort(txn);
leave:
    return code;
}

inline
const char *kvs_GetImplErrDescription(err_t err) {
    return mdb_strerror(err);
}

inline
bool kvs_IsImplErr(err_t err) {
    return err >= MDB_KEYEXIST && err <= MDB_LAST_ERRCODE;
}

err_t kvs_Set(kvs_t *store, const kint_t *key, size_t size, uint8_t *data) {
    assert(store != NULL);
    assert(key != NULL);
    assert(size == 0 || data != NULL);

    int code;
    MDB_txn *txn;
    if ((code = mdb_txn_begin(store->env, NULL, 0, &txn)) != MDB_SUCCESS) {
        if (code == MDB_PANIC) {
            code = ERR_PANIC;
        }
        goto leave;
    }
    MDB_val k = {.mv_size = KDT_B8, .mv_data = (void *) key};
    MDB_val v = {.mv_size = size, .mv_data = data};
    if ((code = mdb_put(txn, store->dbi, &k, &v, 0)) != MDB_SUCCESS) {
        if (code == MDB_MAP_FULL) {
            code = ERR_FULL;
        }
        goto leave_abort_txn;
    }
    code = mdb_txn_commit(txn);
    goto leave;

leave_abort_txn:
    mdb_txn_abort(txn);
leave:
    return code;
}

#else
#error No supported KVS implementation.
#endif