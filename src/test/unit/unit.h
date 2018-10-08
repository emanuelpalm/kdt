/**
 * Unit testing framework.
 *
 * Testing framework for running suites of individual tests. Note that the
 * framework is single-threaded, meaning that all tests are executed
 * synchronously and all framework functions must be called from the same
 * thread the framework was initialized on. If testing asynchronous code, a
 * test in question may not return until after the asynchronous operations have
 * completed.
 *
 * @file unit.h
 */
#ifndef KDT_UNIT_H
#define KDT_UNIT_H

#include <setjmp.h>
#include <stdlib.h>

/**
 * Unit call context macro.
 *
 * This macro expands into a call context which describes the function, file
 * and line at which the macro is placed.
 *
 * The macro is intended to be use together with the _unit_AbortCF(),
 * _unit_FailCF() and _unit_SkipCF() functions, in order for these to be able
 * to properly print the contexts in which they are called.
 */
#define UNIT_C ((unit_C) { __func__, __FILE_REL__, __LINE__ })

typedef struct unit_State unit_State;
typedef struct unit_T unit_T;
typedef struct unit_C unit_C;

/**
 * Global unit test context.
 *
 * An instance of this type represents the results of all performed unit tests.
 */
struct unit_State {
    struct {
        size_t total, failed;
    } suites;
};

/**
 * Unit test suite function pointer.
 */
typedef void (*unit_Suite)(unit_T *);

/**
 * Unit test function pointer.
 *
 * The void pointer argument is only given if a provider function is used, and
 * it passes on anything of relevance.
 */
typedef void (*unit_Test)(unit_T *, void *);

/**
 * Unit test suite context.
 *
 * An instance of this type is passed around to all tests within the same suite
 * in order for them to report on their results. The convention is to name
 * instances of this type T when passing them around.
 */
struct unit_T {
    struct {
        size_t total, failed, skipped;
    } tests;
    struct {
        size_t argno, failed;
    } test;
    jmp_buf jmp_skip, jmp_fatal;
};

/**
 * Unit call context.
 *
 * Represents the function, file and line at which a fail, skip or fatal call
 * is made. The type is meant to be instantiated using the UNIT_X macro.
 */
struct unit_C {
    const char *func;
    const char *file;
    const int line;
};

/**
 * Initializes unit test suite data structure.
 *
 * @param state Uninitialized unit testing state.
 */
void unit_Init(unit_State *state);

/**
 * Reports on unit test suite results and returns exit status.
 *
 * Returns with the EXIT_FAILURE exit code if any test failed. In case all
 * tests passed then EXIT_SUCCESS is provided instead.
 *
 * @param state Initialized unit testing state.
 */
int unit_Term(unit_State *state);

/**
 * Prints name of given suite, runs suite, and reports results.
 *
 * @param state Initialized unit testing state.
 * @param name Human-readable name of executed test suite.
 * @param suite Function pointer to test suite to execute.
 */
void unit_RunSuite(unit_State *state, const char *name, unit_Suite suite);

/**
 * Runs given test and stores any results into T.
 *
 * If args is set, the test function is executed once for every argument. Note
 * that args must be terminated with a NULL pointer.
 *
 * @param T Test suite context pointer.
 * @param t Function pointer to individual test to execute.
 * @param args NULL-terminated list of argument pointers, or NULL.
 */
void unit_RunTest(unit_T *T, unit_Test t, void **args);

/**
 * Skips current test suite, failing it, reporting given message.
 *
 * The execution of the current test suite is stopped. Any clean-up required
 * ought to be performed prior to issuing this call.
 *
 * @param T Test suite context pointer.
 * @param M Message to report.
 */
#define unit_Abort(T, M) unit_AbortCF(T, UNIT_C, M)

/**
 * Skips current test suite, failing it, reporting given message.
 *
 * The execution of the current test suite is stopped. Any clean-up required
 * ought to be performed prior to issuing this call.
 *
 * @param T Test suite context pointer.
 * @param F Format string of reported message.
 * @param ... Format arguments.
 */
#define unit_AbortF(T, F, ...) unit_AbortCF(T, UNIT_C, F, __VA_ARGS__)

/**
 * Skips current test suite, failing it, reporting given message.
 *
 * The execution of the current test suite is stopped. Any clean-up required
 * ought to be performed prior to issuing this call.
 *
 * @param T Test suite context pointer.
 * @param C Function call context.
 * @param F Format string of reported message.
 * @param ... Format arguments.
 */
void unit_AbortCF(unit_T *T, unit_C C, const char *F, ...);

/**
 * Fails current test, reporting given message.
 *
 * Do note that execution of the current function continues after the call.
 *
 * @param T Test suite context pointer.
 * @param M Message to report.
 */
#define unit_Fail(T, M) unit_FailCF(T, UNIT_C, M)

/**
 * Fails current test, reporting given message.
 *
 * Do note that execution of the current function continues after the call.
 *
 * @param T Test suite context pointer.
 * @param F Format string of reported message.
 * @param ... Format arguments.
 */
#define unit_FailF(T, F, ...) unit_FailCF(T, UNIT_C, F, __VA_ARGS__)

/**
 * Fails current test, reporting given message.
 *
 * Do note that execution of the current function continues after the call.
 *
 * @param T Test suite context pointer.
 * @param C Function call context.
 * @param F Format string of reported message.
 * @param ... Format arguments.
 */
void unit_FailCF(unit_T *T, unit_C C, const char *F, ...);

/**
 * Skips current test, reporting given message.
 *
 * The execution of the current test is stopped. Any clean-up required ought to
 * be performed prior to issuing this call.
 *
 * @param T Test suite context pointer.
 * @param M Message to report.
 */
#define unit_Skip(T, M) unit_SkipCF(T, UNIT_C, M)

/**
 * Skips current test, reporting given message.
 *
 * The execution of the current test is stopped. Any clean-up required ought to
 * be performed prior to issuing this call.
 *
 * @param T Test suite context pointer.
 * @param F Format string of reported message.
 * @param ... Format arguments.
 */
#define unit_SkipF(T, F, ...) unit_SkipCF(T, UNIT_C, F, __VA_ARGS__)

/**
 * Skips current test, reporting given message.
 *
 * The execution of the current test is stopped. Any clean-up required ought to
 * be performed prior to issuing this call.
 *
 * @param T Test suite context pointer.
 * @param C Function call context.
 * @param F Format string of reported message.
 * @param ... Format arguments.
 */
void unit_SkipCF(unit_T *T, unit_C C, const char *F, ...);

#endif