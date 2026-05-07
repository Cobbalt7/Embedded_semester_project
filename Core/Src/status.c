#include "status.h"
#include <string.h>

size_t get_error_str(status_t error, char * result) {
    switch (error) {
        case STATUS_OK: {
            strcpy(result, "No error\r\n");
            break;
        }
        case ERR_NULL_ARG: {
            strcpy(result, "Error: null argument\r\n");
            break;
        }
        case ERR_INV_ENUM_VAL: {
            strcpy(result, "Error: invalid enum value\r\n");
            break;
        }
        case ERR_FLASH_ERASE: {
            strcpy(result, "Error: flash erase failed\r\n");
            break;
        }
        case ERR_FLASH_PROGRAM: {
            strcpy(result, "Error: flash program failed\r\n");
            break;
        }
        case ERR_INV_CONF_VAL: {
            strcpy(result, "Error: invalid configuration value\r\n");
            break;
        }
        case ERR_INV_COMMAND: {
            strcpy(result, "Error: invalid command\r\n");
            break;
        }
        default: {
            strcpy(result, "Error: invalid error code");
            break;
        }
    }
    return  strlen(result);
}