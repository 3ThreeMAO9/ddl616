/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_led.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "bsp_led.h"

/*************************Variable*************************/
const bsp_led_drive_io_t bsp_led = {
    .init = led_init,
    .sleep = led_sleep,
    .handle = led_hanlde,
    .loop = led_loop,
};

//--------------------------------------------------
const bsp_led_drive_io_t* bsp_led_get_driver(bsp_led_callback_t callback)
{
    ledEventRegister_callback(callback);
    return &bsp_led;
}

