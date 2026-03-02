/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_sleep.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-22
 */

#include "bsp_sleep.h"

/***************Variable***************/
const bsp_sleep_drive_io_t bsp_sleep = {
    .init = unused_gpio_init,
    .loop = sleep_loop,

};

// ------------------------------------------

const bsp_sleep_drive_io_t* bsp_sleep_get_driver(bsp_sleep_config_callback_t callback)
{
    sleep_event_register_callback(callback);
    return &bsp_sleep;
}
