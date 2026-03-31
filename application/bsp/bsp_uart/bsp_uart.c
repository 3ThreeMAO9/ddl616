/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_uart.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "bsp_uart.h"

/*************************Variable*************************/
const bsp_uart_drive_io_t bsp_uart = {
    .init = module_uart_init,
    .queue_put = module_uart_queue_put,
    .retry_clean = module_uart_retry_clean,
    .is_wake = module_uart_is_wake,
    .loop = module_uart_poll,
    .sleep = module_uart_sleep,
};

//--------------------------------------------------
const bsp_uart_drive_io_t* bsp_uart_get_driver(bsp_uart_callback_t callback)
{
    uartEventRegister_callback(callback);
    return &bsp_uart;
}

