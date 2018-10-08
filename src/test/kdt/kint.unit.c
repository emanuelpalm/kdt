#include <assert.h>
#include <kdt/kint.h>
#include <string.h>
#include <unit/unit.h>

typedef struct ArgCLZ ArgCLZ;
typedef struct ArgCMP ArgCMP;
typedef struct ArgEQU ArgEQU;
typedef struct ArgXOR ArgXOR;

struct ArgCLZ {
    size_t z;
    void *i;
};

struct ArgCMP {
    int d;
    void *a;
    void *b;
};

struct ArgEQU {
    bool e;
    void *a;
    void *b;
};

struct ArgXOR {
    void *d;
    void *a;
    void *b;
};

static void TestCLZ(unit_T *T, void *_arg);
static void TestCMP(unit_T *T, void *_arg);
static void TestEQU(unit_T *T, void *_arg);
static void TestHash(unit_T *T, void *_arg);
static void TestWriteText(unit_T *T, void *_arg);
static void TestRandom(unit_T *T, void *_arg);
static void TestXOR(unit_T *T, void *_arg);

static const ArgCLZ *DATA_CLZ[] = {
    &(ArgCLZ) {
        .z = KDT_B,
        .i = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    &(ArgCLZ) {
        .z = 8,
        .i = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00",
    },
    &(ArgCLZ) {
        .z = 0,
        .i = "\x01\x00\x00\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF",
    },
    &(ArgCLZ) {
        .z = KDT_B - 1,
        .i = "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    &(ArgCLZ) {
        .z = 121,
        .i = "\x70\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    &(ArgCLZ) {
        .z = 4,
        .i = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08",
    },
    &(ArgCLZ) {
        .z = 40,
        .i = "\x00\x00\x00\x00\x00\x00\xc2\xDD\x00\x10\xA0\x00\x00\x00\x00\x00",
    },
    &(ArgCLZ) {
        .z = 52,
        .i = "\x00\x00\x00\x00\x00\x00\xc2\xDD\x00\x0F\x00\x00\x00\x00\x00\x00",
    },
    &(ArgCLZ) {
        .z = 65,
        .i = "\x00\x00\x00\x00\x00\x00\xc2\x40\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    NULL
};

static const ArgCMP *DATA_CMP[] = {
    &(ArgCMP) {
        .d = 0,
        .a = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    &(ArgCMP) {
        .d = 0,
        .a = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF",
        .b = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF",
    },
    &(ArgCMP) {
        .d = 0,
        .a = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
    },
    &(ArgCMP) {
        .d = -1,
        .a = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .b = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
    },
    &(ArgCMP) {
        .d = 1,
        .a = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF",
        .b = "\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00",
    },
    &(ArgCMP) {
        .d = 1,
        .a = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    &(ArgCMP) {
        .d = -1,
        .a = "\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
    },
    NULL
};

static const ArgEQU *DATA_EQU[] = {
    &(ArgEQU) {
        .e = true,
        .a = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    &(ArgEQU) {
        .e = true,
        .a = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF",
        .b = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF",
    },
    &(ArgEQU) {
        .e = false,
        .a = "\xA9\x72\x2B\x88\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
        .b = "\xCD\x11\x3D\x92\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
    },
    NULL
};

static const ArgXOR *DATA_XOR[] = {
    &(ArgXOR) {
        .d = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .a = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    &(ArgXOR) {
        .d = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        .a = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    &(ArgXOR) {
        .d = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        .a = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .b = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
    },
    &(ArgXOR) {
        .d = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .a = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF",
        .b = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF",
    },
    &(ArgXOR) {
        .d = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
        .a = "\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF",
        .b = "\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00",
    },
    &(ArgXOR) {
        .d = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .a = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
    },
    &(ArgXOR) {
        .d = "\x64\x63\x16\x1A\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
        .a = "\xA9\x72\x2B\x88\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
        .b = "\xCD\x11\x3D\x92\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",
    },
    &(ArgXOR) {
        .d = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08",
        .a = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08",
        .b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    },
    NULL
};

void test_kint_unit_c(unit_T *T) {
    unit_RunTest(T, TestCLZ, (void **) DATA_CLZ);
    unit_RunTest(T, TestCMP, (void **) DATA_CMP);
    unit_RunTest(T, TestEQU, (void **) DATA_EQU);
    unit_RunTest(T, TestWriteText, NULL);
    unit_RunTest(T, TestHash, NULL);
    unit_RunTest(T, TestRandom, NULL);
    unit_RunTest(T, TestXOR, (void **) DATA_XOR);
}

static void TestCLZ(unit_T *T, void *_arg) {
    const ArgCLZ *arg = _arg;
    size_t actual = kint_CLZ(arg->i);
    if (arg->z != actual) {
        unit_FailF(T, "Expected %zu, got: %zu.", arg->z, actual);
    }
}

static void TestCMP(unit_T *T, void *_arg) {
    const ArgCMP *arg = _arg;
    int d = kint_CMP((const kint_t *) arg->a, (const kint_t *) arg->b);
    if (d < 0) {
        d = -1;
    }
    else if (d > 0) {
        d = 1;
    }
    if (arg->d != d) {
        unit_FailF(T, "Expected %d; got %d.", arg->d, d);
    }
}

static void TestEQU(unit_T *T, void *_arg) {
    const ArgEQU *arg = _arg;
    bool actual = kint_EQU(arg->a, arg->b);
    if (arg->e != actual) {
        unit_FailF(T, "Expected %d, got: %d.", arg->e, actual);
    }
}

static void TestWriteText(unit_T *T, void *_arg) {
    (void) _arg;

    const kint_t x = (kint_t) {
        .as_u8s = {0xEF, 0xCF, 0xCA, 0x71, 0x62, 0x4F, 0xE1, 0x77,
                  0xAC, 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0xF4}
    };

    char actual[(KDT_B8 * 2) + 1];
    mem_t mem = mem_FromBuffer((uint8_t *) actual, sizeof(actual));

    kint_WriteText(&x, &mem);

    if (mem_Size(&mem) != KDT_B8 * 2) {
        unit_FailF(T, "Expected size %d, got: %d", KDT_B8 * 2, mem_Size(&mem));
        return;
    }

    const char *expected = "EFCFCA71624FE177ACF85E994102DFF4";
    if (memcmp(actual, expected, KDT_B8 * 2) != 0) {
        unit_FailF(T, "Expected: \"%s\"; actual: \"%s\".", expected, actual);
    }
}

static void TestHash(unit_T *T, void *_arg) {
    (void) _arg;

    const uint8_t *expected_hash = (const uint8_t *)
        "\xA9\x57\x09\xEC\xFF\x96\x87\x01\x8A\x94\x59\xB1\x18\x03\xE5\xAD";

    kint_t x = kint_Hash((const uint8_t *) "Hello, Sailor!", 14);
    if (memcmp(&x.as_u8s, expected_hash, sizeof(x.as_u8s)) != 0) {
        unit_Fail(T, "Unexpected hash result.");
    }
}

static void TestRandom(unit_T *T, void *_arg) {
    (void) _arg;

    kint_t x = kint_Random();
    kint_t z = {0};

    if (memcmp(&x.as_u8s, &z.as_u8s, sizeof(x.as_u8s)) == 0) {
        unit_Fail(T, "Returned 0, which probably (?) means it did nothing.");
    }
}

static void TestXOR(unit_T *T, void *_arg) {
    const ArgXOR *arg = _arg;
    const kint_t d = kint_XOR(
        (const kint_t *) arg->a,
        (const kint_t *) arg->b
    );
    if (memcmp(arg->d, &d, sizeof(kint_t)) != 0) {
        unit_Fail(T, "d != a XOR b");
    }
}
