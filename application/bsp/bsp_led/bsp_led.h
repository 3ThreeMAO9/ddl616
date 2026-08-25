/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_led.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef BSP_LED__HH
#define BSP_LED__HH

#include "config.h"
#include "led.h"

typedef led_event_callback_t bsp_led_callback_t;

/*************************Macro*************************/

/*************************Enum*************************/

/*************************Struct*************************/
typedef struct {
    void (*init)(void);
    void (*sleep)(void);
    void (*handle)(uint8_t event,uint16_t value);
    void (*loop)(void);
} bsp_led_drive_io_t;

/*************************Variable*************************/

/*************************Function*************************/
const bsp_led_drive_io_t* bsp_led_get_driver(bsp_led_callback_t callback);

/**********************************************************/

#endif

