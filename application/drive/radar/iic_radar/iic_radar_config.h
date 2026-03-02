/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: iic_radar_config.h
 * Desc: I2C雷达模块硬件配置头文件
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef __IIC_RADAR_CONFIG_H__
#define __IIC_RADAR_CONFIG_H__

#include "config.h"
#include "hal_gpio.h"

/***********Macro***********/
// 雷达模块I2C从机地址（二进制地址右移1位，适配7位地址格式）
#define IIC_RADAR_IIC_ADDR      (0x50 >> 1)
#define I2C_CLK_DELAY_US        (0)         // I2C时钟延时
#define I2C_BYTE_DELAY_US       (5)         // 字节间延时
#define I2C_ACK_TIME_OUT        (100)       // 字节间延时
#define I2C_RETRY_TIMES         (2)         // I2C重试次数

// 雷达I2C引脚电平读取
#define READ_RADAR_INT()     (HAL_GPIO_Read(RADAR_INT_GPIO, RADAR_INT_PIN))


// 雷达I2C SCL引脚初始化（初始电平+引脚模式配置）
#define RADAR_I2C_SCL_INIT(_level)                                                                            \
    do                                                                                                        \
    {                                                                                                         \
        HAL_GPIO_Write(RADAR_I2C_SCL_GPIO, RADAR_I2C_SCL_PIN, _level);                                        \
        HAL_GPIO_Init(RADAR_I2C_SCL_GPIO, RADAR_I2C_SCL_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

// 雷达I2C SDA引脚初始化（初始电平+引脚模式配置）
#define RADAR_I2C_SDA_INIT(_level)                                                                            \
    do                                                                                                        \
    {                                                                                                         \
        HAL_GPIO_Write(RADAR_I2C_SDA_GPIO, RADAR_I2C_SDA_PIN, _level);                                        \
        HAL_GPIO_Init(RADAR_I2C_SDA_GPIO, RADAR_I2C_SDA_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

// 雷达中断引脚初始化（初始电平+输入模式配置）
#define RADAR_INT_INIT(_level)                                                                 \
    do                                                                                         \
    {                                                                                          \
        HAL_GPIO_Write(RADAR_INT_GPIO, RADAR_INT_PIN, _level);                                 \
        HAL_GPIO_Init(RADAR_INT_GPIO, RADAR_INT_PIN, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE); \
    } while (0)

// 雷达中断引脚初始化（初始电平+推挽模式配置）
#define RADAR_INT_PULL_INIT(_level)                                                                   \
    do                                                                                                \
    {                                                                                                 \
        HAL_GPIO_Write(RADAR_INT_GPIO, RADAR_INT_PIN, _level);                                        \
        HAL_GPIO_Init(RADAR_INT_GPIO, RADAR_INT_PIN, HAL_GPIO_MODE_INPUT_PULLUP, HAL_GPIO_PULL_NONE); \
    } while (0)

// 雷达中断引脚中断控制
#define RADAR_INT_PIN_ENABLE()  (HAL_GPIO_EnableIRQ(RADAR_INT_GPIO, RADAR_INT_PIN, HAL_GPIO_IRQ_FALLING))
#define RADAR_INT_PIN_DISABLE() (HAL_GPIO_DisableIRQ(RADAR_INT_GPIO, RADAR_INT_PIN))

// 雷达中断状态管理
#define RADAR_INT_READ_INTSTATE()  (HAL_GPIO_ReadIntState(RADAR_INT_GPIO, RADAR_INT_PIN))
#define RADAR_INT_CLEAR_INTSTATE() (HAL_GPIO_ClearIntState(RADAR_INT_GPIO, RADAR_INT_PIN))

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* __IIC_RADAR_CONFIG_H__ */
