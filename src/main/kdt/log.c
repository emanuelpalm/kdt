#include "log.h"
#include <kdt/def.h>
#include <kdt/mtx.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef NDEBUG
#include <assert.h>
#define _ASSERT_INITIALIZED assert(_initialized != 0);
#else
#define _ASSERT_INITIALIZED
#endif

#define _LOG(CTX, FD, CATEGORY, FORMAT) do {                 \
    _ASSERT_INITIALIZED;                                     \
    log_Context _ctx = (CTX);                                \
    fprintf(FD, CATEGORY " {%s:%d} ", _ctx.file, _ctx.line); \
    va_list args;                                            \
    va_start(args, FORMAT);                                  \
    vfprintf(FD, FORMAT, args);                              \
    va_end(args);                                            \
    puts("");                                                \
} while (0)

#ifndef NDEBUG
static int _initialized = 0;
#endif

static mtx_t _lock;

void log_Init() {
    mtx_Init(&_lock);
#ifndef NDEBUG
    _initialized = 1;
#endif
}

noreturn void log_PanicCF(log_Context context, const char *format, ...) {
    _LOG(context, stderr, MAGENTA("PANIC"), format);
    exit(EXIT_FAILURE);
}

void log_BugCF(log_Context context, const char *format, ...) {
    mtx_Lock(&_lock);
    _LOG(context, stderr, RED("BUG"), format);
    mtx_Unlock(&_lock);
}

void log_WarnCF(log_Context context, const char *format, ...) {
    mtx_Lock(&_lock);
    _LOG(context, stderr, YELLOW("WARNING"), format);
    mtx_Unlock(&_lock);
}

void log_NoteCF(log_Context context, const char *format, ...) {
    mtx_Lock(&_lock);
    _LOG(context, stdout, BLUE("NOTICE"), format);
    mtx_Unlock(&_lock);
}