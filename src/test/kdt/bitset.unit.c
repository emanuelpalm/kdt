#include <kdt/bitset.h>
#include <string.h>
#include <unit/unit.h>

#define _ASSERT_BOOL(T, EXPECTED, ACTUAL) do {             \
    bool _e = (EXPECTED);                                  \
    bool _a = (ACTUAL);                                    \
    if (_e != _a) {                                        \
        unit_FailF((T), "Expected: %d; got: %d.", _e, _a); \
        return;                                            \
    }                                                      \
} while (0)

#define _ASSERT_BUFFER(T, EXPECTED, SIZE, ACTUAL) do {   \
    if (memcmp((EXPECTED), (ACTUAL), (SIZE)) != 0) {     \
        unit_Fail(T, "`" #ACTUAL "` state unexpected."); \
    }                                                    \
} while (0)

static void TestAllocate(unit_T *T, void *_arg);
static void TestSetClear(unit_T *T, void *_arg);

void test_bitset_unit_c(unit_T *T) {
    unit_RunTest(T, TestAllocate, NULL);
    unit_RunTest(T, TestSetClear, NULL);
}

static void TestAllocate(unit_T *T, void *_arg) {
    (void) _arg;

    uint8_t buffer[] = {0x09, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x80};
    bitset_t bitset;
    bitset_Init(&bitset, buffer, sizeof(buffer));

    size_t out[3], out_fail = 200;

    _ASSERT_BOOL(T, true, bitset_Allocate(&bitset, &out[0]));
    _ASSERT_BOOL(T, true, bitset_Allocate(&bitset, &out[1]));
    _ASSERT_BOOL(T, true, bitset_Allocate(&bitset, &out[2]));
    _ASSERT_BOOL(T, false, bitset_Allocate(&bitset, &out_fail));

    for (size_t i = 0; i < sizeof(out) / sizeof(size_t); ++i) {
        size_t index = out[i];
        if (!(index == 0 || index == 3 || index == 87)) {
            unit_FailF(T, "%zu not in {0, 3, 15}.", index);
        }
    }
    if (out_fail != 200) {
        unit_FailF(T, "Expected: 200; got: %zu", out_fail);
    }
}

static void TestSetClear(unit_T *T, void *_arg) {
    (void) _arg;

    uint8_t buffer[] = {0x00, 0x00, 0x00};
    bitset_t bitset = {.bytes = buffer, .size = sizeof(buffer)};

    bitset_Clear(&bitset, 3);
    _ASSERT_BUFFER(T, "\x00\x00\x00", sizeof(buffer), buffer);

    bitset_Set(&bitset, 9);
    _ASSERT_BUFFER(T, "\x00\x02\x00", sizeof(buffer), buffer);

    bitset_Set(&bitset, 22);
    _ASSERT_BUFFER(T, "\x00\x02\x40", sizeof(buffer), buffer);

    bitset_Set(&bitset, 3);
    _ASSERT_BUFFER(T, "\x08\x02\x40", sizeof(buffer), buffer);

    bitset_Clear(&bitset, 22);
    _ASSERT_BUFFER(T, "\x08\x02\x00", sizeof(buffer), buffer);

    bitset_Set(&bitset, 6);
    _ASSERT_BUFFER(T, "\x48\x02\x00", sizeof(buffer), buffer);
}