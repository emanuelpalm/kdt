#include "cli.h"
#include <ctype.h>
#include <errno.h>
#include <kdt/log.h>
#include <string.h>
#include <unistd.h>

err_t _kdm_PollCLI(_kdm_CLI *cli) {
    // Read user input.
    char buffer[1024];
    {
        size_t offset = 0;
        char *cursor;
        do {
            cursor = &buffer[offset];
            ssize_t n = read(STDIN_FILENO, cursor, 1);
            if (n < 0) {
                return errno;
            }
            if (*cursor == '\n') {
                *cursor = '\0';
                break;
            }
            offset += n;
            if (n == 0) {
                buffer[offset] = '\0';
                break;
            }
        } while (offset < (sizeof(buffer) - 1));
    }

    // Split user input.
    const char *argv[3] = {"", "", ""};
    size_t argc = 0;
    {
        for (char *cursor = buffer; *cursor != '\0' && argc < 3; cursor = &cursor[1]) {
            if (isspace(*cursor)) {
                continue;
            }
            if (*cursor == '"') {
                argv[argc] = &cursor[1];
                do {
                    cursor = &cursor[1];
                } while (*cursor != '"' && *cursor != '\0');
            }
            else {
                argv[argc] = cursor;
                while (!isspace(*cursor) && *cursor != '\0') {
                    cursor = &cursor[1];
                }
            }
            argc += 1;
            if (*cursor == '\0') {
                break;
            }
            *cursor = '\0';
        }
    }

    // Execute any matching CLI function.
    if (strcasecmp("help", argv[0]) == 0) {
        if (argc != 1) {
            log_Warn("Usage: help");
        }
        else {
            cli->on_help(cli->data);
        }
    }
    else if (strcasecmp("exit", argv[0]) == 0) {
        if (argc != 1) {
            log_Warn("Usage: exit");
        }
        else {
            cli->on_exit(cli->data);
        }
    }
    else if (strcasecmp("get", argv[0]) == 0) {
        if (argc != 2) {
            log_Warn("Usage: get <key>");
        }
        else {
            cli->on_get(cli->data, argv[1]);
        }
    }
    else if (strcasecmp("join", argv[0]) == 0) {
        if (argc != 2) {
            log_Warn("Usage: join \"<host>\"");
        }
        else {
            cli->on_join(cli->data, argv[1]);
        }
    }
    else if (strcasecmp("set", argv[0]) == 0) {
        if (argc != 3) {
            log_Warn("Usage: set <key> <value>");
        }
        else {
            cli->on_set(cli->data, argv[1], argv[2]);
        }
    }
    else {
        log_WarnF("Unknown command: %s", argv[0]);
    }

    return ERR_NONE;
}