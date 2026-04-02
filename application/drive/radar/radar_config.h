/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: io_radar_config.h
 * Desc: I2C雷达模块硬件配置头文件
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-12
 */

#ifndef __IO_RADAR_CONFIG_H__
#define __IO_RADAR_CONFIG_H__

#include "config.h"
#include "hal_gpio.h"

/***********Macro***********/
// 雷达I2C引脚电平读取
#define READ_RADAR_INT()     (HAL_GPIO_Read(RADAR_INT_GPIO, RADAR_INT_PIN))

// 雷达I2C SCL引脚初始化（初始电平+引脚模式配置）
#define RADAR_PB2_SCL_INIT(_level)                                                                            \
    do                                                                                                        \
    {                                                                                                         \
        HAL_GPIO_Write(RADAR_PB2_SCL_GPIO, RADAR_PB2_SCL_PIN, _level);                                        \
        HAL_GPIO_Init(RADAR_PB2_SCL_GPIO, RADAR_PB2_SCL_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

// 雷达I2C SDA引脚初始化（初始电平+引脚模式配置）
#define RADAR_PB3_SDA_INIT(_level)                                                                            \
    do                                                                                                        \
    {                                                                                                         \
        HAL_GPIO_Write(RADAR_PB3_SDA_GPIO, RADAR_PB3_SDA_PIN, _level);                                        \
        HAL_GPIO_Init(RADAR_PB3_SDA_GPIO, RADAR_PB3_SDA_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

// 雷达中断引脚初始化（初始电平+输入模式配置）
#define RADAR_INT_INIT(_level)                                                                 \
    do                                                                                         \
    {                                                                                          \
        HAL_GPIO_Write(RADAR_INT_GPIO, RADAR_INT_PIN, _level);                                 \
        HAL_GPIO_Init(RADAR_INT_GPIO, RADAR_INT_PIN, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE); \
    } while (0)

#define RADAR_INT_PULL_INIT(_level)                                                                \
    do                                                                                             \
    {                                                                                              \
        HAL_GPIO_Write(RADAR_INT_GPIO, RADAR_INT_PIN, _level);                                     \
        HAL_GPIO_Init(RADAR_INT_GPIO, RADAR_INT_PIN, HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE); \
    } while (0)

// 雷达中断引脚初始化（初始电平+推挽模式配置）
#define RADAR_INT_PULLUP_INIT(_level)                                                                 \
    do                                                                                                \
    {                                                                                                 \
        HAL_GPIO_Write(RADAR_INT_GPIO, RADAR_INT_PIN, _level);                                        \
        HAL_GPIO_Init(RADAR_INT_GPIO, RADAR_INT_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

// 雷达中断引脚中断控制
#define RADAR_INT_PIN_ENABLE()  (HAL_GPIO_EnableIRQ(RADAR_INT_GPIO, RADAR_INT_PIN, HAL_GPIO_IRQ_RISING))
#define RADAR_INT_PIN_DISABLE() (HAL_GPIO_DisableIRQ(RADAR_INT_GPIO, RADAR_INT_PIN))

// 雷达中断状态管理
#define RADAR_INT_READ_INTSTATE()  (HAL_GPIO_ReadIntState(RADAR_INT_GPIO, RADAR_INT_PIN))
#define RADAR_INT_CLEAR_INTSTATE() (HAL_GPIO_ClearIntState(RADAR_INT_GPIO, RADAR_INT_PIN))

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* __IO_RADAR_CONFIG_H__ */
