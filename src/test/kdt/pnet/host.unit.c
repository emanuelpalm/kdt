#include <kdt/pnet/host.h>
#include <string.h>
#include <unit/unit.h>

typedef struct ArgInternetAsString ArgInternetAsString;
typedef struct ArgTransportAsString ArgTransportAsString;
typedef struct ArgWriteHostText ArgWriteHostText;

struct ArgInternetAsString {
    uint8_t i;
    const char *s;
};

struct ArgTransportAsString {
    uint8_t t;
    const char *s;
};

struct ArgWriteHostText {
    pnet_Host *h;
    const char *s;
};

static const ArgInternetAsString *DATA_InternetAsString[] = {
    &(ArgInternetAsString) {.i = 0xFF, .s = NULL},
    &(ArgInternetAsString) {.i = 0xc3, .s = NULL},
    &(ArgInternetAsString) {.i = 0x32, .s = NULL},
    &(ArgInternetAsString) {.i = PNET_INTERNET_IPV4, .s = "IPv4"},
    &(ArgInternetAsString) {.i = PNET_INTERNET_IPV6, .s = "IPv6"},
    NULL
};

static ArgTransportAsString *DATA_TransportAsString[] = {
    &(ArgTransportAsString) {.t = 0xFF, .s = NULL},
    &(ArgTransportAsString) {.t = 0xc3, .s = NULL},
    &(ArgTransportAsString) {.t = 0x32, .s = NULL},
    &(ArgTransportAsString) {.t = PNET_TRANSPORT_TCP, .s = "TCP"},
    NULL
};

static const ArgWriteHostText *DATA_WriteHostText[] = {
    &(ArgWriteHostText) {
        .h = &(pnet_Host) {
            .internet = PNET_INTERNET_NONE,
            .transport = PNET_TRANSPORT_NONE,
            .address = {0},
            .port = 12345,
        },
        .s = "?/? ?:12345",
    },
    &(ArgWriteHostText) {
        .h = &(pnet_Host) {
            .internet = PNET_INTERNET_IPV4,
            .transport = PNET_TRANSPORT_TCP,
            .address = {192, 168, 2, 3},
            .port = 60543,
        },
        .s = "IPv4/TCP 192.168.2.3:60543",
    },
    &(ArgWriteHostText) {
        .h = &(pnet_Host) {
            .internet = PNET_INTERNET_IPV6,
            .transport = PNET_TRANSPORT_TCP,
            .address = {0xFD, 0xE4, 0x8D, 0xBA, 0x82, 0xE1, 0xF1, 0xF2,
                        0xF3, 0xF4, 0x15, 0x26, 0x37, 0x48, 0x59, 0x60},
            .port = 22345,
        },
        .s = "IPv6/TCP [fde4:8dba:82e1:f1f2:f3f4:1526:3748:5960]:22345",
    },
    &(ArgWriteHostText) {
        .h = &(pnet_Host) {
            .internet = PNET_INTERNET_IPV6,
            .transport = PNET_TRANSPORT_TCP,
            .address = {0xFD, 0xE4, 0x8D, 0xBA, 0x82, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x37, 0x48, 0x59, 0x60},
            .port = 1,
        },
        .s = "IPv6/TCP [fde4:8dba:8200::3748:5960]:1",
    },
    NULL
};

static void TestWriteHostText(unit_T *T, void *_arg);
static void TestInternetAsString(unit_T *T, void *_arg);
static void TestTransportAsString(unit_T *T, void *_arg);

void test_pnet_host_unit_c(unit_T *T) {
    unit_RunTest(T, TestInternetAsString, (void **) DATA_InternetAsString);
    unit_RunTest(T, TestTransportAsString, (void **) DATA_TransportAsString);
    unit_RunTest(T, TestWriteHostText, (void **) DATA_WriteHostText);
}

static void TestInternetAsString(unit_T *T, void *_arg) {
    const ArgInternetAsString *arg = _arg;
    const char *actual = pnet_GetInternetDescription(arg->i);
    if (actual == arg->s) {
        return;
    }
    if (actual == NULL || arg->s == NULL || strcmp(arg->s, actual) != 0) {
        unit_FailF(T, "Expected: \"%s\"; actual: \"%s\".", arg->s, actual);
    }
}

static void TestTransportAsString(unit_T *T, void *_arg) {
    const ArgTransportAsString *arg = _arg;
    const char *actual = pnet_GetTransportDescription(arg->t);
    if (actual == arg->s) {
        return;
    }
    if (actual == NULL || arg->s == NULL || strcmp(arg->s, actual) != 0) {
        unit_FailF(T, "Expected: \"%s\"; actual: \"%s\".", arg->s, actual);
    }
}

static void TestWriteHostText(unit_T *T, void *_arg) {
    const ArgWriteHostText *arg = _arg;
    char buffer[256];
    mem_t mem = mem_FromBuffer((uint8_t *) buffer, sizeof(buffer));

    const err_t err = pnet_WriteHostText(arg->h, &mem);
    if (err != ERR_NONE) {
        unit_FailF(T, "Unexpected format error: %s", err_GetDescription(err));
        return;
    }
    mem_Write8(&mem, 0x00);

    if (strcmp(arg->s, buffer) != 0) {
        unit_FailF(T, "Expected: \"%s\"; actual: \"%s\".", arg->s, buffer);
    }
}