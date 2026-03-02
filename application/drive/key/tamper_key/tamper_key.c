/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: tamper_key.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-22
 */

#include "tamper_key.h"
#include "hal_gpio.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "tamper_key"

uint32_t tamper_key_init_value = 0; // 初始化的时候，防撬键的电平状态

void tamper_key_init(void)
{
    TAMPER_KEY_INIT(1);

    tamper_key_init_value = READ_TAMPER_KEY_LEVEL();
    if(tamper_key_init_value)
        TAMPER_KEY_IRQ_FALLING();
    else
        TAMPER_KEY_IRQ_RISING();
}

void tamper_key_sleep(uint8_t event)
{
    if (ENTER_SLEEP == event){

    }
    else{

    }
}

uint8_t read_tamper_key_level(uint8_t id)
{
    if (READ_TAMPER_KEY_LEVEL() == tamper_key_init_value){
        return true;
    }
    return false;
}

uint16_t tamper_key_check_wake(void)
{
    if (READ_TAMPER_KEY_STATE())
    {
        CLEAR_TAMPER_KEY_STATE();
        if(tamper_key_init_value == READ_TAMPER_KEY_LEVEL())
            return true;
    }
    return false;
}


