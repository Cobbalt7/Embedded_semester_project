#ifndef INC_STATUS_H
#define INC_STATUS_H

#include <stddef.h>

typedef enum status {
    STATUS_OK = 0,
    ERR_NULL_ARG = -1,
    ERR_INV_ENUM_VAL = -2,
    ERR_FLASH_ERASE = -3,
    ERR_FLASH_PROGRAM = -4,
    ERR_INV_CONF_VAL = -5,
    ERR_INV_COMMAND = -6,
    STATUS_FIRST = STATUS_OK,
    STATUS_LAST,//TODO
} status_t;

#define RETURN_IF_NOT_OK(expr) \
    do {                       \
        status_t temp = expr;  \
        if(temp != STATUS_OK) {\
            return temp;       \
        }                      \
    } while(0)

/**
 * @brief Get the error string by error code.
 * 
 * @note result array has to have at least 39 bytes of memory.
 * @param error Error code.
 * @param result Pointer to array to store string.
 * @return size_t Size of error string save in result.
 */
size_t get_error_str(status_t error, char * result);

#endif /* INC_ERR_H */