#include "err.h"
#include "kvs.h"
#include <string.h>

const char *err_GetDescription(err_t err) {
    switch (err) {
    case ERR_NONE:
        return "No error";

    case ERR_FAILED:
        return "Operation failed";

    case ERR_FULL:
        return "Not enough space for resource";

    case ERR_NOT_COMPATIBLE:
        return "Resource incompatible";

    case ERR_NOT_FOUND:
        return "Resource not found";

    case ERR_NOT_VALID:
        return "Resource invalid";

    case ERR_PANIC:
        return "Operation panic";

    case ERR_TOO_LARGE:
        return "Resource too large";

    case ERR_TOO_SMALL:
        return "Resource too small";

    case ERR_TRY_AGAIN:
        return "Try again";

    case ERR_TIMEOUT:
        return "Operation timed out";

    default:
        break;
    }
    if (err > 0) {
        return strerror(err);
    }
    if (kvs_IsImplErr(err)) {
        return kvs_GetImplErrDescription(err);
    }
    return "Unknown error";
}