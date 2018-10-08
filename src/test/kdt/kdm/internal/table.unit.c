#include <kdt/kdm/internal/table.h>
#include <string.h>
#include <unit/unit.h>

typedef struct ArgGetBucket ArgGetBucket;

struct ArgGetBucket {
    kint_t *o;
    kint_t *i;
    size_t p;
};

static const ArgGetBucket *DATA_GetBucket[] = {
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .p = 16,
    },
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .p = KDT_B1 - 1,
    },
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .p = KDT_B1 - 1,
    },
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0xEF, 0xCF, 0xCA, 0x71, 0x62, 0x4F, 0xE1, 0x77,
                0xAC, 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0xF4
            }
        },
        .p = 0,
    },
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
                0xAC, 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0xF4
            }
        },
        .p = 0,
    },
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0xED, 0x8A, 0xFF, 0xF2, 0x32, 0xF3, 0x68, 0x04,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0xAC, 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0x74,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .p = 1,
    },
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .p = 54,
    },
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0x00, 0xFF, 0xCA, 0x71, 0x62, 0x4F, 0xE1, 0x77,
                0xAC, 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0xF4
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0x00, 0xFF, 0xCA, 0x71, 0x62, 0x4F, 0xE1, 0x77,
                0xAC, 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0xF4
            }
        },
        .p = KDT_B1 - 1,
    },
    &(ArgGetBucket) {
        .o = &(kint_t) {
            .as_u8s = {
                0xAC, 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0xF4,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .i = &(kint_t) {
            .as_u8s = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xDF, 0xF4,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            }
        },
        .p = 25,
    },
    NULL
};

static void TestGetBucket(unit_T *T, void *_arg);

void test_kdm_internal_table_unit_c(unit_T *T) {
    unit_RunTest(T, TestGetBucket, (void **) DATA_GetBucket);
}

static void TestGetBucket(unit_T *T, void *_arg) {
    const ArgGetBucket *arg = _arg;

    _kdm_Table table;
    _kdm_InitTable(&table, arg->o);

    const _kdm_Bucket *actual = _kdm_GetBucket(&table, arg->i);
    const _kdm_Bucket *expected = &table.buckets[arg->p];
    if (expected != actual) {
        unit_FailF(T, "Expected: %zu; got: %zu.", arg->p, actual - &table.buckets[0]);
    }
}