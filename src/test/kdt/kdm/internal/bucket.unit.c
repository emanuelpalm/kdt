#include <kdt/kdm/internal/bucket.h>
#include <string.h>
#include <unit/unit.h>

#define _CONTACT(N) {                      \
    .id = _ID(N),                          \
    .host = {                              \
        .internet = PNET_INTERNET_IPV4,    \
        .transport = PNET_TRANSPORT_TCP,   \
        .address = {192, 168, 2, (N) + 1}, \
        .port = 60543,                     \
    },                                     \
}
#define _ID(N) {                                       \
    .as_u8s = {                                        \
        (N), 0x00, 0x00, 0x71, 0x62, 0x4F, 0xE1, 0x77, \
        (N), 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0xF4  \
    }                                                  \
}

typedef struct ArgGetKthContact ArgGetKthContact;
typedef struct ArgPushContact ArgPushContact;
typedef struct ArgRemoveContactWithID ArgRemoveContactWithID;

struct ArgGetKthContact {
    kdm_Contact *c;
};

struct ArgPushContact {
    kdm_Contact *c;
    uint8_t p[KDT_K];
};

struct ArgRemoveContactWithID {
    kint_t i;
    uint8_t p[KDT_K];
};

// Tests operate on copies of this bucket.
static const _kdm_Bucket BUCKET = {
    .contacts = {_CONTACT(1), _CONTACT(2), _CONTACT(3)}
};

static const ArgGetKthContact *DATA_GetKthContact[] = {
    &(ArgGetKthContact) {
        .c = &(kdm_Contact) _CONTACT(4),
    },
    &(ArgGetKthContact) {
        .c = NULL,
    },
    NULL
};

static const ArgPushContact *DATA_PushContact[] = {
    &(ArgPushContact) {
        .c = &(kdm_Contact) _CONTACT(4),
        .p = {4, 1, 2, 3},
    },
    &(ArgPushContact) {
        .c = &(kdm_Contact) _CONTACT(3),
        .p = {3, 1, 2, 0},
    },
    &(ArgPushContact) {
        .c = &(kdm_Contact) _CONTACT(2),
        .p = {2, 1, 3, 0},
    },
    &(ArgPushContact) {
        .c = &(kdm_Contact) _CONTACT(1),
        .p = {1, 2, 3, 0},
    },
    NULL
};

static const ArgRemoveContactWithID *DATA_RemoveContactWithID[] = {
    &(ArgRemoveContactWithID) {
        .i = _ID(1),
        .p = {2, 3, 0, 0},
    },
    &(ArgRemoveContactWithID) {
        .i = _ID(2),
        .p = {1, 3, 0, 0},
    },
    &(ArgRemoveContactWithID) {
        .i = _ID(3),
        .p = {1, 2, 0, 0},
    },
    &(ArgRemoveContactWithID) {
        .i = _ID(4),
        .p = {1, 2, 3, 0},
    },
    NULL
};

static void TestGetKthContact(unit_T *T, void *_arg);
static void TestPushContact(unit_T *T, void *_arg);
static void TestRemoveContactWithID(unit_T *T, void *_arg);

void test_kdm_internal_bucket_unit_c(unit_T *T) {
    unit_RunTest(T, TestGetKthContact, (void **) DATA_GetKthContact);
    unit_RunTest(T, TestPushContact, (void **) DATA_PushContact);
    unit_RunTest(T, TestRemoveContactWithID, (void **) DATA_RemoveContactWithID);
}

static void TestGetKthContact(unit_T *T, void *_arg) {
    const ArgGetKthContact *arg = _arg;
    kdm_Contact *expected;

    _kdm_Bucket bucket;
    memcpy(&bucket, &BUCKET, sizeof(_kdm_Bucket));
    if (arg->c != NULL) {
        expected = &bucket.contacts[KDT_K - 1];
        memcpy(expected, arg->c, sizeof(kdm_Contact));
    }
    else {
        expected = NULL;
    }
    const kdm_Contact *actual = _kdm_GetKthContact(&bucket);
    if (expected != actual) {
        unit_FailF(T, "Expected: %p; got: %p.", expected, actual);
    }
}

static void TestPushContact(unit_T *T, void *_arg) {
    const ArgPushContact *arg = _arg;

    _kdm_Bucket bucket;
    memcpy(&bucket, &BUCKET, sizeof(_kdm_Bucket));

    _kdm_PushContact(&bucket, arg->c);

    kdm_Contact *p = bucket.contacts;
    for (size_t i = 0; i < KDT_K; ++i) {
        if (arg->p[i] != p[i].id.as_u8s[0]) {
            unit_FailF(
                T, "Expected: {%d %d %d %d}; got: {%d %d %d %d}.",
                arg->p[0], arg->p[1], arg->p[2], arg->p[3],
                p[0].id.as_u8s[0], p[1].id.as_u8s[0], p[2].id.as_u8s[0], p[3].id.as_u8s[0]
            );
            break;
        }
    }
}

static void TestRemoveContactWithID(unit_T *T, void *_arg) {
    const ArgRemoveContactWithID *arg = _arg;

    _kdm_Bucket bucket;
    memcpy(&bucket, &BUCKET, sizeof(_kdm_Bucket));

    _kdm_RemoveContactWithID(&bucket, &arg->i);

    kdm_Contact *p = bucket.contacts;
    for (size_t i = 0; i < KDT_K; ++i) {
        if (arg->p[i] != p[i].id.as_u8s[0]) {
            unit_FailF(
                T, "Expected: {%d %d %d %d}; got: {%d %d %d %d}.",
                arg->p[0], arg->p[1], arg->p[2], arg->p[3],
                p[0].id.as_u8s[0], p[1].id.as_u8s[0], p[2].id.as_u8s[0], p[3].id.as_u8s[0]
            );
            break;
        }
    }
}