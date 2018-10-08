#ifndef KDT_TIMS_H
#define KDT_TIMS_H

/**
 * Time, in seconds, since the UNIX epoch.
 */
typedef double tims_t;

/**
 * @return Current time, in seconds since the epoch.
 */
tims_t tims_Now();

#endif
