#include <kdt/cbuf.h>
#include <unit/unit.h>

#define _ASSERT_PUSH(T, CBUFZ, VALUE, SUCCESS) do {        \
    bool _e = (SUCCESS);                                   \
    bool _a = cbufz_Push((CBUFZ), (VALUE));                \
    if (_e != _a) {                                        \
        unit_FailF((T), "Expected: %d; got: %d.", _e, _a); \
        return;                                            \
    }                                                      \
} while (0)

#define _ASSERT_POP(T, CBUFZ, VALUE, SUCCESS) do {           \
    size_t _v = (VALUE);                                     \
    size_t _o;                                               \
    bool _e = (SUCCESS);                                     \
    bool _a = cbufz_Pop((CBUFZ), &_o);                       \
    if (_e != _a) {                                          \
        unit_FailF((T), "Expected: %d; got: %d.", _e, _a);   \
        return;                                              \
    }                                                        \
    if (_a && _v != _o) {                                    \
        unit_FailF((T), "Expected: %zu; got: %zu.", _v, _o); \
        return;                                              \
    }                                                        \
} while (0)

static void TestPushPop(unit_T *T, void *_arg);

void test_cbuf_unit_c(unit_T *T) {
    unit_RunTest(T, TestPushPop, NULL);
}

static void TestPushPop(unit_T *T, void *_arg) {
    (void) _arg;

    size_t buffer[5];
    cbufz_t cbufz;
    cbufz_Init(&cbufz, buffer, sizeof(buffer) / sizeof(size_t));

    _ASSERT_PUSH(T, &cbufz, 100, true);
    _ASSERT_PUSH(T, &cbufz, 200, true);
    _ASSERT_PUSH(T, &cbufz, 300, true);
    _ASSERT_PUSH(T, &cbufz, 400, true);
    _ASSERT_PUSH(T, &cbufz, 500, false);

    _ASSERT_POP(T, &cbufz, 100, true);
    _ASSERT_POP(T, &cbufz, 200, true);

    _ASSERT_PUSH(T, &cbufz, 500, true);
    _ASSERT_PUSH(T, &cbufz, 600, true);
    _ASSERT_PUSH(T, &cbufz, 700, false);

    _ASSERT_POP(T, &cbufz, 300, true);
    _ASSERT_POP(T, &cbufz, 400, true);
    _ASSERT_POP(T, &cbufz, 500, true);
    _ASSERT_POP(T, &cbufz, 600, true);
    _ASSERT_POP(T, &cbufz, 700, false);

    _ASSERT_PUSH(T, &cbufz, 700, true);
}