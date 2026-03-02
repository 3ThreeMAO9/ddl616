
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: fingerprint_config.h
 * Desc: 指纹/ 指静脉模块硬件配置
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-10
 */
#ifndef __FINGERPRINT_CONFIG_H
#define __FINGERPRINT_CONFIG_H

#include "config.h"
#include "hal_uart.h"
#include "hal_gpio.h"

/*****************Macro****************/
#define FINGER_UART_BAUDRATE                UART_BAUDRATE_57600

#define FINGERPRINT_UART_TIMEOUT            (10) // unit: ms

#define SET_FINGERPRINT_POWER(_enable)      (HAL_GPIO_Write(FP_POWER_GPIO, FP_POWER_PIN, (_enable ? 0 : 1)))
#define SET_FINGERPRINT_WAKE(_level)        (HAL_GPIO_Write(FP_WAKE_GPIO, FP_WAKE_PIN, _level))
#define SET_FINGERPRINT_TX(_level)          (HAL_GPIO_Write(FP_TX_GPIO, FP_TX_PIN, _level))
#define SET_FINGERPRINT_RX(_level)          (HAL_GPIO_Write(FP_RX_GPIO, FP_RX_PIN, _level))

#define FINGERPRINT_POWER_INIT(_enable)                                                          \
    do                                                                                           \
    {                                                                                            \
        HAL_GPIO_Write(FP_POWER_GPIO, FP_POWER_PIN, _enable);                                    \
        HAL_GPIO_Init(FP_POWER_GPIO, FP_POWER_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define FINGERPRINT_WAKE_INIT(_level)                                                      \
    do                                                                                     \
    {                                                                                      \
        HAL_GPIO_Write(FP_WAKE_GPIO, FP_WAKE_PIN, _level);                                 \
        HAL_GPIO_Init(FP_WAKE_GPIO, FP_WAKE_PIN, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE); \
    } while (0)

#define FINGERPRINT_TX_INIT(_level)                                                        \
    do                                                                                     \
    {                                                                                      \
        HAL_GPIO_Write(FP_TX_GPIO, FP_TX_PIN, _level);                                     \
        HAL_GPIO_Init(FP_TX_GPIO, FP_TX_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define FINGERPRINT_RX_INIT(_level)                                                        \
    do                                                                                     \
    {                                                                                      \
        HAL_GPIO_Write(FP_RX_GPIO, FP_RX_PIN, _level);                                     \
        HAL_GPIO_Init(FP_RX_GPIO, FP_RX_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)


#define FINGERPRINT_WAKE_ENABLE()               (HAL_GPIO_EnableIRQ(FP_WAKE_GPIO, FP_WAKE_PIN, HAL_GPIO_IRQ_RISING))
#define FINGERPRINT_WAKE_DISABLE()              (HAL_GPIO_DisableIRQ(FP_WAKE_GPIO, FP_WAKE_PIN))
#define READ_FINGERPRINT_WAKE()                 (HAL_GPIO_Read(FP_WAKE_GPIO, FP_WAKE_PIN))

#define FINGERPRINT_WAKE_READ_INTSTATE()        (HAL_GPIO_ReadIntState(FP_WAKE_GPIO, FP_WAKE_PIN))
#define FINGERPRINT_WAKE_CLEAR_INTSTATE()       (HAL_GPIO_ClearIntState(FP_WAKE_GPIO, FP_WAKE_PIN))

/*****************Enum*****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/


/**************************************/

#endif /* __FINGERPRINT_CONFIG_H */
