#ifndef KDT_KDM_CONTACT_H
#define KDT_KDM_CONTACT_H

#include "kdt/kint.h"
#include <kdt/pnet/host.h>
#include <kdt/err.h>
#include <kdt/mem.h>
#include <stdbool.h>

typedef struct kdm_Contact kdm_Contact;

/**
 * A Kademlia network contact.
 */
struct kdm_Contact {
    /// Contact identifier.
    kint_t id;

    /// Contact host.
    pnet_Host host;
};

/**
 * Writes textual representation of contact to memory region.
 *
 * @param contact Contact to write.
 * @param mem Memory region to write to.
 * @return ERR_NONE only if operation succeeded.
 */
err_t kdm_WriteContactText(const kdm_Contact *contact, mem_t *mem);

/**
 * Checks whether given contact is empty, meaning it does not currently
 * represent any Kademlia network participant.
 *
 * @param bucket Contact to check.
 * @return Whether contact is empty.
 */
bool kdm_IsContactEmpty(const kdm_Contact *contact);

#endif
