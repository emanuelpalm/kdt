#include "contact.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define _TRY(ERR) do {        \
    const err_t _err = (ERR); \
    if (_err != ERR_NONE) {   \
        return _err;          \
     }                        \
} while (0)

err_t kdm_WriteContactText(const kdm_Contact *contact, mem_t *mem) {
    assert(contact != NULL);
    assert(mem != NULL);

    _TRY(mem_WriteF(mem, "Contact{id:"));
    kint_WriteText(&contact->id, mem);

    _TRY(mem_WriteF(mem, ",host:"));
    pnet_WriteHostText(&contact->host, mem);

    _TRY(mem_WriteF(mem, "}"));

    return ERR_NONE;
}

inline
bool kdm_IsContactEmpty(const kdm_Contact *contact) {
    return contact->host.internet == PNET_INTERNET_NONE;
}