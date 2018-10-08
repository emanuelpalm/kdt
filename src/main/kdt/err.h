/**
 * Various utilities related to representing application errors.
 */
#ifndef KDT_ERR_H
#define KDT_ERR_H

/**
 * The error type.
 *
 * Positive values are used to represent system errors, while negative values
 * are used for application errors. ERR_NONE, which is defined as 0, is used to
 * convey that an operation succeeded.
 */
typedef int err_t;

enum {
    ERR_NONE = (err_t) 0,            ///< No error.
    ERR_FAILED = (err_t) -1,         ///< Operation failed to complete.
    ERR_FULL = (err_t) -2,           ///< Not enough space is available for resource.
    ERR_NOT_COMPATIBLE = (err_t) -3, ///< Resource not compatible.
    ERR_NOT_FOUND = (err_t) -4,      ///< Resource not found.
    ERR_NOT_VALID = (err_t) -5,      ///< Resource not in expected state.
    ERR_PANIC = (err_t) -6,          ///< Operation failed. Restart or exit application.
    ERR_TOO_LARGE = (err_t) -7,      ///< Resource too large for operation to complete.
    ERR_TOO_SMALL = (err_t) -8,      ///< Resource too small for operation to complete.
    ERR_TRY_AGAIN = (err_t) -9,      ///< Resource busy. Try again later.
    ERR_TIMEOUT = (err_t) -10,       ///< Operation timed out.
};

/**
 * Provides string representation of error.
 *
 * @param err Error to represent.
 * @return String representation. Never returns NULL.
 */
const char *err_GetDescription(err_t err);

#endif
