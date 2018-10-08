#include "kdm.h"

#include "internal/cli.h"
#include "internal/protocol.h"
#include "internal/table.h"
#include <ctype.h>
#include <errno.h>
#include <kdt/pnet/pnet.h>
#include <kdt/def.h>
#include <kdt/kint.h>
#include <kdt/kvs.h>
#include <kdt/log.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define _TRY(ERR) do {        \
    const err_t _err = (ERR); \
    if (_err != ERR_NONE) {   \
        return _err;          \
     }                        \
} while (0)

static struct {
    /// Kademlia protocol handler.
    _kdm_Protocol protocol;

    /// Worker threads.
    pthread_t threads[KDT_THREADS];
} _kdm;

static
void OnUserHelp(void *data);

static
void OnUserExit(void *data);

static
void OnUserGet(void *data, const char *key);

static
void OnUserJoin(void *data, const char *host_str);

static
void OnUserSet(void *data, const char *key, const char *value);

static
void *StartWorker(void *_arg);

err_t kdm_Start(kvs_t *store, pnet_t *pnet, const pnet_Host *peer) {
    // Setup and report.
    {
        _kdm.protocol.running = true;

        _TRY(_kdm_InitProtocol(&_kdm.protocol, store, pnet));

        uint8_t _mem[64];
        mem_t mem = mem_FromBuffer(_mem, sizeof(_mem));

        kint_t *client_id = _kdm_GetClientID(&_kdm.protocol);
        mem_WriteX(&mem, (const uint8_t *) client_id, sizeof(kint_t));
        log_NoteF("Node ID: %s", mem.begin);

        mem_Reset(&mem);
        _TRY(pnet_WriteHostText(pnet_GetInterface(pnet), &mem));
        log_NoteF("Accepting connections on interface: %s", mem.begin);
    }

    // Spawn worker threads.
    for (size_t i = KDT_THREADS; i-- != 0;) {
        _TRY(pthread_create(&_kdm.threads[i], NULL, StartWorker, &_kdm.protocol));
    }
    log_NoteF("Using %zu worker threads.", KDT_THREADS);

    // Start network join attempt via `peer`.
    {
        _TRY(_kdm_Join(&_kdm.protocol, peer));
    }

    // Read/handle CLI input.
    log_Note("Type "
                 BLUE("`help`")
                 " for a list of available commands.");
    {
        _kdm_CLI cli = {
            .data = &_kdm.protocol,
            .on_help = OnUserHelp,
            .on_exit = OnUserExit,
            .on_get = OnUserGet,
            .on_join = OnUserJoin,
            .on_set = OnUserSet,
        };
        while (atomic_load(&_kdm.protocol.running)) {
            err_t err = _kdm_PollCLI(&cli);
            switch (err) {
            case ERR_NONE:
                continue;

            case EINTR:
                return ERR_NONE;

            default:
                log_WarnF("CLI Error: %s (%d)", err_GetDescription(err), err);
                return err;
            }
        }
    }

    return ERR_NONE;
}

static
void OnUserHelp(void *data) {
    (void) data;

    log_Note(
        "Available commands:\n"
        "  help              - Show this message.\n"
        "  exit              - Shutdown application.\n"
        "  get <key>         - Perform key lookup.\n"
        "  join <host>       - Connect to another host in the same network.\n"
        "  set <key> <value> - Store key/value pair.\n"
        "  -------\n"
        "  Enclose any command parameter with quotes (\") if needing to use spaces."
    );
}

static
void OnUserExit(void *data) {
    (void) data;

    kdm_Shutdown();
}

static
void OnUserGet(void *data, const char *key) {
    (void) data;

    // TODO: Implement.
    log_NoteF("KEY: %s", key);
}

static
void OnUserJoin(void *data, const char *host_str) {
    _kdm_Protocol *protocol = data;

    mem_t host_mem = mem_FromString((char *) host_str);
    pnet_Host host;

    err_t err = pnet_ReadHostText(&host_mem, &host);
    if (err != ERR_NONE) {
        log_WarnF("Invalid peer address; %s.", err_GetDescription(err));
        log_Note("Example: IPv4/TCP 127.0.0.1:19002");
        return;
    }
    _kdm_Join(protocol, &host);
}

static
void OnUserSet(void *data, const char *key, const char *value) {
    (void) data;

    // TODO: Implement.
    log_NoteF("KEY: %s; VALUE: %s", key, value);
}

void kdm_Shutdown() {
    bool expected = true;
    while (!atomic_compare_exchange_weak(&_kdm.protocol.running, &expected, false)) {
        if (!atomic_load(&_kdm.protocol.running)) {
            break;
        }
    }
    log_Note("Waiting for worker threads to exit ...");
    for (size_t i = KDT_THREADS; i-- != 0;) {
        pthread_join(_kdm.threads[i], NULL);
    }
    log_Note("All worker threads exited.");
}

static
void *StartWorker(void *_arg) {
    _kdm_Protocol *protocol = _arg;

    struct timespec out;
    while (atomic_load(&protocol->running)) {
        err_t err;
        if ((err = _kdm_Poll(protocol)) == ERR_NONE) {
            continue;
        }
        long sleep_nsec;
        if (err == ERR_NOT_FOUND) {
            sleep_nsec = 30000000;
            goto sleep;
        }
        if (err == ERR_TRY_AGAIN) {
            sleep_nsec = 20000000;
            goto sleep;
        }
        log_WarnF("Poll failed. Reason: %s (%d)", err_GetDescription(err), err);
        sleep_nsec = 10000000;
sleep:
        nanosleep(&(struct timespec) {.tv_nsec = sleep_nsec, .tv_sec = 0}, &out);
    }
    return NULL;
}