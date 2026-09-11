
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

#define SET_FINGERPRINT_POWER_HIGH()        (FP_POWER_GPIO->DATA |= FP_POWER_PIN)
#define SET_FINGERPRINT_POWER_LOW()         (FP_POWER_GPIO->DATA &= ~FP_POWER_PIN)

#define SET_FINGERPRINT_POWER_ON()          (SET_FINGERPRINT_POWER_LOW())
#define SET_FINGERPRINT_POWER_OFF()         (SET_FINGERPRINT_POWER_HIGH())

#define SET_FINGERPRINT_WAKE_HIGH()         (FP_WAKE_GPIO->DATA |= FP_WAKE_PIN)
#define SET_FINGERPRINT_WAKE_LOW()          (FP_WAKE_GPIO->DATA &= ~FP_WAKE_PIN)

#define SET_FINGERPRINT_TX(_level)          (HAL_GPIO_Write(FP_TX_GPIO, FP_TX_PIN, _level))
#define SET_FINGERPRINT_RX(_level)          (HAL_GPIO_Write(FP_RX_GPIO, FP_RX_PIN, _level))

#define FINGERPRINT_POWER_INIT()                                                                   \
    do                                                                                             \
    {                                                                                              \
        SET_FINGERPRINT_POWER_OFF();                                                               \
        GPIO_SetPinMFType(FP_POWER_GPIO, FP_POWER_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_PUSH_PULL); \
    } while (0)

#define FINGERPRINT_WAKE_INIT()                                                              \
    do                                                                                       \
    {                                                                                        \
        SET_FINGERPRINT_WAKE_HIGH();                                                         \
        GPIO_SetPinMFType(FP_WAKE_GPIO, FP_WAKE_PIN, GPIO_MF_TYPE_GPIO, GPIO_PINMODE_INPUT); \
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

#define READ_FINGERPRINT_WAKE()                 (FP_WAKE_GPIO->PIN & FP_WAKE_PIN)

#define FINGERPRINT_WAKE_READ_INTSTATE()        (hal_read_gpio_int_flag(FP_WAKE_GPIO, FP_WAKE_PIN))
#define FINGERPRINT_WAKE_CLEAR_INTSTATE()       (hal_clear_gpio_int_flag(FP_WAKE_GPIO, FP_WAKE_PIN))

#define FINGERPRINT_WAKE_ENABLE()                                            \
    {                                                                        \
        GPIO_EnableINT(FP_WAKE_GPIO, FP_WAKE_PIN, GPIO_INTMODE_RISING_EDGE); \
        FINGERPRINT_WAKE_CLEAR_INTSTATE();                                   \
    }

#define FINGERPRINT_WAKE_DISABLE()                                       \
    {                                                                    \
        GPIO_EnableINT(FP_WAKE_GPIO, FP_WAKE_PIN, GPIO_INTMODE_DISABLE); \
        FINGERPRINT_WAKE_CLEAR_INTSTATE();                               \
    }

/*****************Enum*****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/


/**************************************/

#endif /* __FINGERPRINT_CONFIG_H */
