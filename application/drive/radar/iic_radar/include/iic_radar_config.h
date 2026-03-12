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

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif /* __IIC_RADAR_CONFIG_H__ */
