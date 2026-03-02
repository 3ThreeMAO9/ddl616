/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: iic_radar_driver.h
 * Desc: AT10LP雷达模块底层I2C驱动头文件（私有，仅API层调用）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef __IIC_RADAR_DRIVER_H__
#define __IIC_RADAR_DRIVER_H__

#include "stdint.h"
#include "stdbool.h"
#include "i2c_software.h"

/***********Macro***********/

/***********Enum***********/

/***********Struct***********/
// 雷达寄存器读写结构体（单寄存器操作）
typedef struct
{
    uint8_t reg_addr;  // 寄存器地址
    uint8_t reg_value; // 寄存器值
} radar_register_t;

// 雷达模块运行状态句柄（记录核心运行参数）
typedef struct
{
    uint8_t init_flag : 1;       // 初始化标志位（0-未初始化，1-已初始化）
    uint8_t reset_period : 6;    // 复位周期计数器（用于周期检测）
    uint8_t error_cnt;           // I2C通信错误计数
    uint8_t existence_flag;      // 雷达芯片存在标志（0-不存在，1-存在）
    uint8_t threshold;           // 各通道检测阈值
    uint8_t gain;                // 各通道增益值
} radar_handle_t;

/***********Function***********/
uint8_t AT10LP_WriteOneByte(uint8_t address, uint8_t data);
void radar_driver_init(void);
void radar_driver_uninit(void);
/*****************************/

#endif /* __IIC_RADAR_DRIVER_H__ */
