#ifndef INC_DEVICE_CONFIG_H
#define INC_DEVICE_CONFIG_H

#include <stdint.h>
#include "status.h"

#define GET_SET_CONF_HANDLER_NAME(name) set_conf_##name##_handler

/**
 * @brief Add configuration parameters here. The syntax is:
 * CONF_XMACRO(NAME, INITIAL VALUE, HANDLER NAME)
 * @note Modify CONFIG_FIRST or CONFIG_LAST if first or last elemnets in
 * CONFIG_PARAMETERS have changed. Also you have to add a set handler to
 * device_config.c
 */
#define CONFIG_PARAMETERS \
    CONF_XMACRO(CONF_MEAS_PERIOD_MS, 200, GET_SET_CONF_HANDLER_NAME(m_period)) \
    CONF_XMACRO(CONF_MEAS_OSS, 3, GET_SET_CONF_HANDLER_NAME(m_oss)) \
    CONF_XMACRO(CONF_USB_MEAS_NUM, 5, GET_SET_CONF_HANDLER_NAME(usb_m_num)) \
    CONF_XMACRO(CONF_DISPLAY_MEAS_NUM, 25, GET_SET_CONF_HANDLER_NAME(d_m_num)) \
    CONF_XMACRO(CONF_BMP180_ADDR, 0xEE, GET_SET_CONF_HANDLER_NAME(bmp180_addr)) \
    CONF_XMACRO(CONF_LCD1602_ADDR, 0x4E, GET_SET_CONF_HANDLER_NAME(lcd1602_addr)) \

#define CONFIG_FIRST CONF_MEAS_PERIOD_MS
#define CONFIG_LAST CONF_LCD1602_ADDR

typedef enum config_param {
    #define CONF_XMACRO(param, unused1, unused2) param,
    CONFIG_PARAMETERS
    #undef CONF_XMACRO
    CONF_FIRST = CONFIG_FIRST,
    CONF_LAST = CONFIG_LAST,
} config_param_t;
#undef CONFIG_FIRST
#undef CONFIG_LAST

/**
 * @brief Initializes configuration. Loads all configuration parameters
 *        from flash.
 * 
 * @return status_t 
 */
status_t config_init();

/**
 * @brief Get the configuration parameter value by enum.
 * 
 * @param param Configuration enum.
 * @param value Pointer to variable where result will be stored.
 * @return status_t 
 */
status_t get_config_param(config_param_t param, uint32_t *value);

/**
 * @brief Set the configuration parameter value by enum.
 * 
 * @param param Configuration enum.
 * @param value Value of configuration parameter.
 * @return status_t 
 */
status_t set_config_param(config_param_t param, uint32_t value);

/**
 * @brief Save current configuration to flash.
 * @note Deletes and overwrites flash pages so use sparingly
 *       for flash longevity.
 * 
 * @return status_t 
 */
status_t save_config();

/**
 * @brief Executes configuration command received in string form.
 *        Syntax: (Configuration parameter enums name)=(Value)
 * 
 * @param command Command string.
 * @return status_t 
 */
status_t execute_config_command(char *command);

#endif /* INC_DEVICE_CONFIG_H */