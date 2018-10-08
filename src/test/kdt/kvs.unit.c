#include <kdt/err.h>
#include <kdt/kvs.h>
#include <string.h>
#include <unit/unit.h>

#define _KEY(N) &(kint_t) {                            \
    .as_u8s = {                                        \
        (N), 0x0F, 0xF0, 0x34, 0x63, 0x2F, 0xE2, 0x00, \
        (N), 0xFF, 0xFE, 0x9F, 0xF1, 0xF2, 0xFF, 0xF2  \
    }                                                  \
}

#define _TRY(T, CODE) do {                                                         \
    err_t _c = (CODE);                                                             \
    if (_c != ERR_NONE) {                                                          \
        unit_FailF((T), "Expected: 0; got: %d (%s).", _c, err_GetDescription(_c)); \
        return;                                                                    \
    }                                                                              \
} while (0)

#define _TRY_ERR(T, EXPECTED, ACTUAL) do {                           \
    err_t _e = (EXPECTED);                                           \
    err_t _a = (ACTUAL);                                             \
    if (_e != _a) {                                                  \
        unit_FailF((T), "Expected: %d (%s); got: %d (%s).",          \
            _e, err_GetDescription(_e), _a, err_GetDescription(_a)); \
        return;                                                      \
    }                                                                \
} while (0)

static void TestCRUD(unit_T *T, void *_arg);

void test_kvs_unit_c(unit_T *T) {
    unit_RunTest(T, TestCRUD, NULL);
}

static void TestCRUD(unit_T *T, void *_arg) {
    (void) _arg;

    kvs_t kvs;
    _TRY(T, kvs_Open("__test_kvs", &kvs));

    _TRY(T, kvs_Set(&kvs, _KEY(1), sizeof("0") - 1, (uint8_t *) "0"));
    _TRY(T, kvs_Set(&kvs, _KEY(2), sizeof("AB") - 1, (uint8_t *) "AB"));
    _TRY(T, kvs_Set(&kvs, _KEY(3), sizeof("mnopqrst") - 1, (uint8_t *) "mnopqrst"));
    _TRY(T, kvs_Set(&kvs, _KEY(4), sizeof("xyz") - 1, (uint8_t *) "xyz"));

    uint8_t buffer[129] = {0};
    mem_t mem = mem_FromBuffer(buffer, sizeof(buffer) - 1);

    _TRY(T, kvs_Get(&kvs, _KEY(2), &mem));
    _TRY(T, kvs_Get(&kvs, _KEY(4), &mem));
    _TRY(T, kvs_Get(&kvs, _KEY(2), &mem));
    _TRY(T, kvs_Get(&kvs, _KEY(1), &mem));
    _TRY(T, kvs_Get(&kvs, _KEY(3), &mem));

    const char *expected = "ABxyzAB0mnopqrst";
    if (strcmp(expected, (const char *) mem.begin) != 0) {
        unit_FailF(T, "Expected: \"%s\"; actual: \"%s\".", expected, mem.begin);
    }

    mem_Reset(&mem);

    _TRY(T, kvs_Delete(&kvs, _KEY(3)));
    _TRY(T, kvs_Delete(&kvs, _KEY(4)));

    _TRY(T, kvs_Get(&kvs, _KEY(1), &mem));
    _TRY(T, kvs_Get(&kvs, _KEY(1), &mem));
    _TRY(T, kvs_Get(&kvs, _KEY(2), &mem));

    _TRY_ERR(T, ERR_NOT_FOUND, kvs_Get(&kvs, _KEY(0), &mem));
    _TRY_ERR(T, ERR_NOT_FOUND, kvs_Get(&kvs, _KEY(3), &mem));
    _TRY_ERR(T, ERR_NOT_FOUND, kvs_Get(&kvs, _KEY(4), &mem));
    _TRY_ERR(T, ERR_NOT_FOUND, kvs_Get(&kvs, _KEY(5), &mem));

    _TRY(T, kvs_Drop(&kvs));
}