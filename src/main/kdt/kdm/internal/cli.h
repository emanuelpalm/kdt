#ifndef KDT_KDM_INTERNAL_CLI_H
#define KDT_KDM_INTERNAL_CLI_H

#include <kdt/err.h>
#include <stdbool.h>

typedef struct _kdm_CLI _kdm_CLI;

struct _kdm_CLI {
    void *data;
    void (*on_help)(void *data);
    void (*on_exit)(void *data);
    void (*on_get)(void *data, const char *key);
    void (*on_join)(void *data, const char *host);
    void (*on_set)(void *data, const char *key, const char *value);
};

/**
 * Polls Command-Line Interface (CLI) for user input. In case of any relevant
 * user input being available, any matching callback function is invoked.
 *
 * @note This function blocks until user input becomes available, or the
 * operation is interrupted by SIGINT.
 *
 * @param cli Pointer to CLI structure.
 */
err_t _kdm_PollCLI(_kdm_CLI *cli);

#endif
