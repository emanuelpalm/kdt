#include "unit/unit.h"

void test_kdm_internal_bucket_unit_c(unit_T *T);
void test_kdm_contact_unit_c(unit_T *T);
void test_kdm_internal_table_unit_c(unit_T *T);
void test_pnet_host_unit_c(unit_T *T);
void test_bitset_unit_c(unit_T *T);
void test_cbuf_unit_c(unit_T *T);
void test_kint_unit_c(unit_T *T);
void test_kvs_unit_c(unit_T *T);

int main() {
    unit_State state;
    unit_Init(&state);

    unit_RunSuite(&state, "test/kdm/internal/bucket.unit.c",
                  test_kdm_internal_bucket_unit_c);
    unit_RunSuite(&state, "test/kdm/internal/table.unit.c",
                  test_kdm_internal_table_unit_c);
    unit_RunSuite(&state, "test/kdm/contact.unit.c", test_kdm_contact_unit_c);
    unit_RunSuite(&state, "test/pnet/host.unit.c", test_pnet_host_unit_c);
    unit_RunSuite(&state, "test/bitset.unit.c", test_bitset_unit_c);
    unit_RunSuite(&state, "test/cbuf.unit.c", test_cbuf_unit_c);
    unit_RunSuite(&state, "test/kint.unit.c", test_kint_unit_c);
    unit_RunSuite(&state, "test/kvs.unit.c", test_kvs_unit_c);

    return unit_Term(&state);
}