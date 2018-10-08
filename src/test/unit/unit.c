#include "unit.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#define BLUE    "\x1B[34m"
#define GREEN   "\x1B[32m"
#define RED     "\x1B[31m"
#define YELLOW  "\x1B[33m"
#define MAGENTA "\x1B[35m"

#ifndef UNIT_NO_COLOR
#define _COLOR(COLOR, STRING) COLOR STRING "\x1B[0m"
#else
#define _COLOR(COLOR, STRING) STRING
#endif

#define _PRINT_CONTEXT(T0, COLOR, KIND, X0) do {                  \
    const unit_T *T1 = (T0);                                      \
    const unit_C *X1 = (X0);                                      \
    const char *format = T1->test.argno > 0                       \
        ? "\t" _COLOR(COLOR, KIND " %s") "#%zu @ %s:%d\n"         \
        : "\t" _COLOR(COLOR, KIND " %s") "%.0zu @ %s:%d\n";       \
    printf(format, X1->func, T1->test.argno, X1->file, X1->line); \
} while (0)

inline void unit_Init(unit_State *state) {
    state->suites.failed = 0;
    state->suites.total = 0;
}

inline int unit_Term(unit_State *state) {
    return state->suites.failed == 0
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}

static void ReportSuccess(unit_T *T);
static void ReportSkipped(unit_T *T);
static void ReportFailure(unit_T *T);

void unit_RunSuite(unit_State *state, const char *name, unit_Suite suite) {
    state->suites.total++;
    printf(_COLOR(BLUE, ">>") " %s\n", name);

    unit_T T = {.test.failed = 0};
    if (setjmp(T.jmp_fatal) == 0) {
        suite(&T);
    }
    if (T.tests.failed == 0 && T.tests.skipped == 0) {
        ReportSuccess(&T);
    }
    else if (T.tests.failed == 0) {
        ReportSkipped(&T);
    }
    else {
        state->suites.failed++;
        ReportFailure(&T);
    }
    putchar('\n');
}

static void ReportSuccess(unit_T *T) {
    printf(_COLOR(GREEN, "<<")
               " Passed all " _COLOR(GREEN, "%zu") " tests.\n",
           T->tests.total);
}

static void ReportSkipped(unit_T *T) {
    printf(_COLOR(YELLOW, "<<")
               " Passed " _COLOR(GREEN, "%zu")
               " and skipped " _COLOR(YELLOW, "%zu")
               " out of " _COLOR(BLUE, "%zu") " tests.\n",
           (T->tests.total - T->tests.skipped),
           T->tests.skipped,
           T->tests.total);
}

static void ReportFailure(unit_T *T) {
    printf(_COLOR(RED, "<<")
               " Failed " _COLOR(RED, "%zu")
               " and skipped " _COLOR(YELLOW, "%zu")
               " out of " _COLOR(BLUE, "%zu") " tests.\n",
           T->tests.failed,
           T->tests.skipped,
           T->tests.total);
}

void unit_RunTest(unit_T *T, unit_Test t, void **args) {
    assert(T != NULL);
    assert(t != NULL);

    if (setjmp(T->jmp_skip) == 0) {
        if (args != NULL) {
            for (; *args != NULL; args = &args[1]) {
                T->test.argno++;
                T->tests.total++;
                t(T, *args);
            }
        }
        else {
            T->tests.total++;
            t(T, NULL);
        }
        if (T->test.failed != 0) {
            T->tests.failed++;
            putchar('\n');
        }
    }
    else {
        for (; *args != NULL; args = &args[1]) {
            T->tests.total++;
        }
    }
    T->test.argno = 0;
    T->test.failed = 0;
}

void unit_AbortCF(unit_T *T, unit_C C, const char *F, ...) {
    _PRINT_CONTEXT(T, MAGENTA, "<= ABORT:", &C);

    T->tests.failed++;

    fputs("\t\t", stdout);
    va_list args;
    va_start(args, F);
    vprintf(F, args);
    va_end(args);
    putchar('\n');

    longjmp(T->jmp_fatal, 1);
}

void unit_FailCF(unit_T *T, unit_C C, const char *F, ...) {
    _PRINT_CONTEXT(T, RED, "-- FAIL:", &C);

    T->test.failed++;

    fputs("\t\t", stdout);
    va_list args;
    va_start(args, F);
    vprintf(F, args);
    va_end(args);
    putchar('\n');
}

void unit_SkipCF(unit_T *T, unit_C C, const char *F, ...) {
    _PRINT_CONTEXT(T, YELLOW, "<- SKIP:", &C);

    T->tests.skipped++;

    fputs("\t\t", stdout);
    va_list args;
    va_start(args, F);
    vprintf(F, args);
    va_end(args);
    putchar('\n');

    longjmp(T->jmp_skip, 1);
}