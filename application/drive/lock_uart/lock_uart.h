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
#define READ_LOCK_UART_INT() (HAL_GPIO_Read(UART_INT_GPIO, UART_INT_PIN))

// RX引脚初始化（初始电平+引脚模式配置）
#define LOCK_UART_PULLUP_INIT(_level)                                                               \
    do                                                                                              \
    {                                                                                               \
        HAL_GPIO_Write(UART_INT_GPIO, UART_INT_PIN, _level);                                        \
        HAL_GPIO_Init(UART_INT_GPIO, UART_INT_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

// RX引脚中断控制
#define LOCK_UART_INT_PIN_ENABLE() (HAL_GPIO_EnableIRQ(UART_INT_GPIO, UART_INT_PIN, HAL_GPIO_IRQ_FALLING))
#define LOCK_UART_INT_PIN_DISABLE() (HAL_GPIO_DisableIRQ(UART_INT_GPIO, UART_INT_PIN))

// RX引脚中断状态管理
#define LOCK_UART_INT_READ_INTSTATE() (HAL_GPIO_ReadIntState(UART_INT_GPIO, UART_INT_PIN))
#define LOCK_UART_INT_CLEAR_INTSTATE() (HAL_GPIO_ClearIntState(UART_INT_GPIO, UART_INT_PIN))

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* __LOCK_UART_H__ */
