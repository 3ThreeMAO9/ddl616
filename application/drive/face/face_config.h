/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: face_config.h
 * Desc: 人脸识别模块硬件配置
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 */
#ifndef __FACE_CONFIG_H
#define __FACE_CONFIG_H

#include "config.h"
#include "hal_uart.h"
#include "hal_gpio.h"

/*****************Macro****************/
// 串口波特率
#define FACE_UART_BAUDRATE                  UART_BAUDRATE_115200

// 串口超时时间（单位：ms）
#define FACE_UART_TIMEOUT                   (10) 

// 基础硬件操作宏
#define SET_FACE_POWER(_enable)             (HAL_GPIO_Write(FACE_POWER_GPIO, FACE_POWER_PIN, (_enable ? 1 : 0)))
#define SET_FACE_TX(_level)                 (HAL_GPIO_Write(FACE_TX_GPIO, FACE_TX_PIN, _level))
#define SET_FACE_RX(_level)                 (HAL_GPIO_Write(FACE_RX_GPIO, FACE_RX_PIN, _level))

// 硬件初始化宏
#define FACE_POWER_INIT(_enable)                                                                     \
    do                                                                                               \
    {                                                                                                \
        HAL_GPIO_Write(FACE_POWER_GPIO, FACE_POWER_PIN, _enable);                                    \
        HAL_GPIO_Init(FACE_POWER_GPIO, FACE_POWER_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)


#define FACE_TX_INIT(_level)                                                                   \
    do                                                                                         \
    {                                                                                          \
        HAL_GPIO_Write(FACE_TX_GPIO, FACE_TX_PIN, _level);                                     \
        HAL_GPIO_Init(FACE_TX_GPIO, FACE_TX_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

#define FACE_RX_INIT(_level)                                                                   \
    do                                                                                         \
    {                                                                                          \
        HAL_GPIO_Write(FACE_RX_GPIO, FACE_RX_PIN, _level);                                     \
        HAL_GPIO_Init(FACE_RX_GPIO, FACE_RX_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

/*****************Enum*****************/


/****************Struct****************/


/***************Variable***************/


/***************Function***************/


/**************************************/

#endif /* __FACE_CONFIG_H */
