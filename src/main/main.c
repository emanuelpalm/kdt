#include <kdt/err.h>
#include <kdt/kdm/kdm.h>
#include <kdt/kvs.h>
#include <kdt/log.h>
#include <kdt/options.h>
#include <kdt/pnet/pnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define _TRY(ERR) do {                                         \
    const err_t _err = (ERR);                                  \
    if (_err != ERR_NONE) {                                    \
        log_PanicF("%s (%d)", err_GetDescription(_err), _err); \
     }                                                         \
} while (0)

static void OnSignal(int signal);

kvs_t store;
pnet_t pnet;

int main() {
    err_t err;

    log_Init();

#ifdef KDT_USE_POSIX
    // Prevent SA_RESTART from being set on POSIX systems.
    sigaction(SIGINT, &(struct sigaction) {.sa_handler = OnSignal }, NULL);
#else
    signal(SIGINT, OnSignal);
#endif

    options_t options;
    if ((err = options_FromEnvironment(&options)) != ERR_NONE) {
        log_Note("Usage: KDT_PEER=\"<peer>\" KDT_INTERFACE=\"<interface>\" kdt");
        goto panic;
    }

    _TRY(kvs_Open("data", &store));
    _TRY(pnet_Open(&pnet, &options.interface));
    _TRY(kdm_Start(&store, &pnet, &options.peer));

    pnet_Close(&pnet);
    kvs_Close(&store);

    log_Note("Bye!");
    exit(EXIT_SUCCESS);

panic:
    log_PanicF("%s (%d)", err_GetDescription(err), err);
}

static void OnSignal(int signal) {
    if (signal == SIGINT) {
        kdm_Shutdown();
    }
}