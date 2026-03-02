/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: gtx314l.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef __GTX314L_H__
#define __GTX314L_H__

#include "config.h"
#include "hal_gpio.h"

/***********Macro***********/
#define TOUCH_IC_CH_CNT (TOUCH_CH_CNT)
#define TOUCH_IIC_ERROR_CNT_MAX (20)
#define TOUCH_RESET_SCAN_PERIOD (25)

/***********Enum***********/

/***********Struct***********/
typedef struct
{
    uint8_t reg;
    uint8_t value;
} touch_register_t;

typedef struct
{
    uint8_t init : 1;
    uint8_t reset_period : 6;
    uint8_t error_cnt;
    uint8_t sensity[TOUCH_IC_CH_CNT];
} touch_handle_t;
/***********Variable***********/

/***********Function***********/
uint8_t touch_init(uint8_t *sensity);
uint8_t touch_scan(uint8_t *touch_value);
uint8_t touch_sleep_config(void);
uint8_t touch_wake_config(uint8_t touch_wake);
uint8_t touch_set_sensity(uint8_t *sensity);
uint8_t is_touch_wake(void);

/*****************************/

#endif /* __GTX314L_H__ */
