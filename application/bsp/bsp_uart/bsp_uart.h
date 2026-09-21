/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_uart.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-05
 */

#ifndef BSP_UART__HH
#define BSP_UART__HH

#include "config.h"
#include "module_uart.h"

typedef uart_event_callback_t bsp_uart_callback_t;

/*************************Macro*************************/

/*************************Enum*************************/

/*************************Struct*************************/
typedef struct {
    void (*init)(void);
    void (*queue_put)(uint8_t *, uint16_t, uint8_t, uint16_t);
    uint8_t (*retry_clean)(uint8_t, uint16_t);
    uint8_t (*is_wake)(void);
    void (*loop)(void);
    void (*sleep)(void);
} bsp_uart_drive_io_t;

/*************************Variable*************************/

/*************************Function*************************/
const bsp_uart_drive_io_t* bsp_uart_get_driver(bsp_uart_callback_t callback);

/**********************************************************/

#endif // BSP_UART__HH

