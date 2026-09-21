/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_uart.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

 #ifndef TASK_UART_H
 #define TASK_UART_H
#include "bsp_uart.h"


/***********Macro***********/
typedef bsp_uart_callback_t task_uart_callback_t;


/***********Enum***********/

/***********Struct***********/
typedef struct{
    uint8_t enable;
}uart_task_attribute_t;

typedef struct{
    const bsp_uart_drive_io_t *io;
    uart_task_attribute_t attribute;

}uart_task_driver_t;

/***********Variable***********/

/***********Function***********/
void uartTaskInit(void);
void uartTaskQueuePut(uint8_t *data, uint16_t tsn, uint8_t cmd, uint16_t len);
uint8_t uartTaskRetryClean(uint8_t cmd, uint16_t tsn);
void uartTaskWake(void);
void uartTaskSleep(void);
void uartTaskLoop(void);
uint8_t uart_task_is_wake(void);
/*****************************/

 #endif // TASK_UART_H

