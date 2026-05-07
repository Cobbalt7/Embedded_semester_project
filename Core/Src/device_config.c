#include "device_config.h"
#include "status.h"
#include "stm32f1xx_hal.h"
#include "Statechart.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_SIZE_KB 1U
#define CONFIG_FLASH_ADDR (FLASH_BANK1_END - CONFIG_SIZE_KB*FLASH_PAGE_SIZE + 1U)
#define MAX_VAL_16BIT 0xffffU
#define MAX_VAL_32BIT 0xffffffffU
#define FLASH_ERASE_VAL MAX_VAL_32BIT

#define ELEM_NUM(array) (sizeof(array)/sizeof(array[0]))
#define SET_CONF_VAL(param, val) config_table[param].value = val
#define GET_CONF_VAL(param) config_table[param].value
#define CONF_HANDLER(param, val) config_table[param].handler(val)

extern TIM_HandleTypeDef htim2;
extern Statechart sc_handle;

typedef struct conf_table_param {
    uint32_t value;
    status_t (*handler)(uint32_t);
} conf_table_param_t;

#define CONF_XMACRO(unused1, unused2, handler) static status_t handler(uint32_t val);
CONFIG_PARAMETERS
#undef CONF_XMACRO

static conf_table_param_t config_table[] = {
    #define CONF_XMACRO(param, init_val, handler) [param] = {init_val, handler},
    CONFIG_PARAMETERS
    #undef CONF_XMACRO
};

static status_t read_flash(uint32_t addr, conf_table_param_t *buff, uint16_t word_num) {
    if(addr == 0 || buff == NULL) {
        return ERR_NULL_ARG;
    }
    for(int i = 0; i < word_num; i++) {
        uint32_t temp = ((volatile uint32_t *)addr)[i];

        // If flash has erase value, retain current(default) value
        if(temp == FLASH_ERASE_VAL) {
            continue;
        }
        buff[i].value=temp;
    }
    return STATUS_OK;
}
static status_t write_flash(uint32_t addr, conf_table_param_t *table, uint16_t word_num) {
    FLASH_EraseInitTypeDef erase_init_struct;
    uint32_t page_err;
    erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init_struct.PageAddress = addr;
    erase_init_struct.NbPages = ((addr + word_num * 4) - addr) / FLASH_PAGE_SIZE + 1;

    HAL_FLASH_Unlock();

    if(HAL_FLASHEx_Erase(&erase_init_struct, &page_err) != HAL_OK) {
        return ERR_FLASH_ERASE;
    }
    for(int i = 0; i < word_num;  i++) {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, table[i].value) != HAL_OK) {
            return ERR_FLASH_PROGRAM;
        }
        addr += 4;
    }

    HAL_FLASH_Lock();

    return STATUS_OK;
}

status_t config_init() {
    RETURN_IF_NOT_OK(read_flash(CONFIG_FLASH_ADDR, config_table, ELEM_NUM(config_table)));
    for(int i = 0; i < ELEM_NUM(config_table); i++) {
        CONF_HANDLER(i, config_table[i].value);
    }
    return STATUS_OK;
}

status_t get_config_param(config_param_t param, uint32_t *value) {
    if(param > CONF_LAST) {
        return ERR_INV_ENUM_VAL;
    }
    if(value == NULL) {
        return ERR_NULL_ARG;
    }
    *value = GET_CONF_VAL(param);
    return STATUS_OK;
}
status_t set_config_param(config_param_t param, uint32_t value) {
    if(param > CONF_LAST) {
        return ERR_INV_ENUM_VAL;
    }
    RETURN_IF_NOT_OK(CONF_HANDLER(param, value));
    return STATUS_OK;
}

status_t save_config() {
    RETURN_IF_NOT_OK(write_flash(CONFIG_FLASH_ADDR, config_table, ELEM_NUM(config_table)));
    return STATUS_OK;
}

status_t execute_config_command(char *command) {
    char *ptr_to_value=strchr(command, '=');
    if(ptr_to_value != NULL) {
        *ptr_to_value='\0';
        ptr_to_value++;
        config_param_t parameter;
        #define CONF_XMACRO(param, unused1, unused2) if(strcmp(command,#param) == 0) {parameter = param;} else
        CONFIG_PARAMETERS {
            parameter = CONF_LAST + 1;
        }
        #undef CONF_XMACRO
        uint32_t value = strtoul(ptr_to_value, NULL, 10);
        if(value == MAX_VAL_32BIT) {
            return ERR_INV_CONF_VAL;
        }
        RETURN_IF_NOT_OK(set_config_param(parameter, value));
    } else {
        return ERR_INV_COMMAND;
    }
    return STATUS_OK;
}

static status_t GET_SET_CONF_HANDLER_NAME(m_period)(uint32_t val) {
    if((val * 2) > MAX_VAL_16BIT || val == 0) {
        return ERR_INV_CONF_VAL;
    }
    HAL_TIM_Base_Stop_IT(&htim2);
    __HAL_TIM_SET_AUTORELOAD(&htim2, (val * 2));
    __HAL_TIM_SET_COUNTER(&htim2, 0U);
    HAL_TIM_Base_Start_IT(&htim2);
    SET_CONF_VAL(CONF_MEAS_PERIOD_MS, val);
    return STATUS_OK;
}

static status_t GET_SET_CONF_HANDLER_NAME(m_oss)(uint32_t val) {
    if(val > 3) {
        return ERR_INV_CONF_VAL;
    }
    SET_CONF_VAL(CONF_MEAS_OSS, val);
    return STATUS_OK;
}

static status_t GET_SET_CONF_HANDLER_NAME(usb_m_num)(uint32_t val) {
    if(val == 0) {
        return ERR_INV_CONF_VAL;
    }
    statechart_set_p_samples_usb(&sc_handle, val);
    SET_CONF_VAL(CONF_USB_MEAS_NUM, val);
    return STATUS_OK;
}

static status_t GET_SET_CONF_HANDLER_NAME(d_m_num)(uint32_t val) {
    if(val == 0) {
        return ERR_INV_CONF_VAL;
    }
    statechart_set_p_samples_disp(&sc_handle, val);
    SET_CONF_VAL(CONF_DISPLAY_MEAS_NUM, val);
    return STATUS_OK;
}

static status_t GET_SET_CONF_HANDLER_NAME(bmp180_addr)(uint32_t val) {
    if(val > 0xFF) {
        return ERR_INV_CONF_VAL;
    }
    SET_CONF_VAL(CONF_BMP180_ADDR, val);
    return STATUS_OK;
}

static status_t GET_SET_CONF_HANDLER_NAME(lcd1602_addr)(uint32_t val) {
    if(val > 0xFF) {
        return ERR_INV_CONF_VAL;
    }
    SET_CONF_VAL(CONF_LCD1602_ADDR, val);
    return STATUS_OK;
}