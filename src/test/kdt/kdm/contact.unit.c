#include <kdt/kdm/contact.h>
#include <string.h>
#include <unit/unit.h>

typedef struct ArgWriteContactText ArgWriteContactText;

struct ArgWriteContactText {
    kdm_Contact *c;
    const char *s;
};

static const ArgWriteContactText *DATA_WriteContactText[] = {
    &(ArgWriteContactText) {
        .c = &(kdm_Contact) {
            .id = {
                .as_u8s = {
                    0xEF, 0xCF, 0xCA, 0x71, 0x62, 0x4F, 0xE1, 0x77,
                    0xAC, 0xF8, 0x5E, 0x99, 0x41, 0x02, 0xDF, 0xF4
                }
            },
            .host = {
                .internet = PNET_INTERNET_IPV4,
                .transport = PNET_TRANSPORT_TCP,
                .address = {192, 168, 2, 3},
                .port = 60543,
            },
        },
        .s = "Contact{"
             "id:EFCFCA71624FE177ACF85E994102DFF4,"
             "host:IPv4/TCP 192.168.2.3:60543}",
    },
    &(ArgWriteContactText) {
        .c = &(kdm_Contact) {
            .id = {
                .as_u8s = {
                    0x00, 0x00, 0x00, 0x71, 0x00, 0x00, 0x00, 0x77,
                    0x00, 0x00, 0x3E, 0x29, 0x31, 0x00, 0x5F, 0x34
                }
            },
            .host = {
                .internet = PNET_INTERNET_IPV6,
                .transport = PNET_TRANSPORT_TCP,
                .address = {0xFD, 0xE4, 0x8D, 0xBA, 0x82, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x37, 0x48, 0x59, 0x60},
                .port = 2,
            },
        },
        .s = "Contact{"
             "id:000000710000007700003E2931005F34,"
             "host:IPv6/TCP [fde4:8dba:8200::3748:5960]:2}",
    },
    NULL
};

static void TestWriteContactText(unit_T *T, void *_arg);

void test_kdm_contact_unit_c(unit_T *T) {
    unit_RunTest(T, TestWriteContactText, (void **) DATA_WriteContactText);
}

static void TestWriteContactText(unit_T *T, void *_arg) {
    const ArgWriteContactText *arg = _arg;
    char buffer[256];
    mem_t mem = mem_FromBuffer((uint8_t *) buffer, sizeof(buffer));

    const err_t err = kdm_WriteContactText(arg->c, &mem);
    if (err != ERR_NONE) {
        unit_FailF(T, "Unexpected format error: %s", err_GetDescription(err));
        return;
    }
    mem_Write8(&mem, 0x00);

    if (strcmp(arg->s, buffer) != 0) {
        unit_FailF(T, "Expected: \"%s\"; actual: \"%s\".", arg->s, buffer);
    }
}