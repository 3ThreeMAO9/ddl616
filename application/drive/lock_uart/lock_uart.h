/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: lock_uart.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-31
 */

#ifndef __LOCK_UART_H__
#define __LOCK_UART_H__

#include "config.h"
#include "hal_gpio.h"

/***********Macro***********/
// RX引脚电平读取
#define READ_LOCK_UART_INT()                (UART_INT_GPIO->PIN & UART_INT_PIN)

// RX引脚初始化（初始电平+引脚模式配置）
#define LOCK_UART_PULLUP_INIT()                                                                  \
    do                                                                                           \
    {                                                                                            \
        UART_INT_GPIO->DATA |= UART_INT_PIN;                                                     \
        GPIO_SetPinMFType(UART_INT_GPIO, UART_INT_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PULL_UP); \
    } while (0)

// RX引脚中断控制
#define LOCK_UART_INT_PIN_ENABLE()          {GPIO_EnableINT(UART_INT_GPIO, UART_INT_PIN, GPIO_INTMODE_FALLING_EDGE);}
#define LOCK_UART_INT_PIN_DISABLE()         {GPIO_EnableINT(UART_INT_GPIO, UART_INT_PIN, GPIO_INTMODE_DISABLE);}

// RX引脚中断状态管理
#define LOCK_UART_INT_READ_INTSTATE()       (hal_read_gpio_int_flag(UART_INT_GPIO, UART_INT_PIN))
#define LOCK_UART_INT_CLEAR_INTSTATE()      (hal_clear_gpio_int_flag(UART_INT_GPIO, UART_INT_PIN))

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* __LOCK_UART_H__ */
