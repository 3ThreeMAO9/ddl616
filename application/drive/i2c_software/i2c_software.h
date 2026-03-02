/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: i2c_software.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef _I2C_SOFTWARE_H_
#define _I2C_SOFTWARE_H_

#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "hal_gpio.h"

/************************Macro************************/

/************************Enum************************/
// 错误码定义
typedef enum
{
    I2C_OK = 0,      // 操作成功
    I2C_ERR_NACK,    // 无应答错误
    I2C_ERR_TIMEOUT, // 超时错误
    I2C_ERR_BUS,     // 总线错误
    I2C_ERR_PARAM,   // 参数错误
} I2C_Result;

/************************Struct************************/
// GPIO操作函数结构体
typedef struct
{
    void (*set_scl)(bool high);    // 设置SCL引脚电平
    void (*set_sda)(bool high);    // 设置SDA引脚电平
    bool (*get_sda)(void);         // 获取SDA引脚电平
    void (*delay_us)(uint16_t us); // 微秒延时函数
} I2C_GPIO_Ops;

// I2C配置结构体
typedef struct
{
    I2C_GPIO_Ops ops;        // GPIO操作函数
    uint8_t slave_addr;      // 从机地址
    uint8_t clock_delay_us;  // 时钟延时
    uint8_t byte_delay_us;   // 字节间延时
    uint8_t ack_timeout;     // ack等待超时
    uint8_t retry_times : 4; // 失败重传次数(0~15)
} I2C_Config;

/************************Variable************************/

/************************Function************************/
// ----------- 基础API -----------
void I2C_Init(const I2C_Config *config);
I2C_Result I2C_Start(const I2C_Config *config);
void I2C_Stop(const I2C_Config *config);
I2C_Result I2C_WriteByte(const I2C_Config *config, uint8_t byte);
I2C_Result I2C_ReadByte(const I2C_Config *config, uint8_t *byte, bool ack);

// ----------- 高级API -----------
I2C_Result I2C_Write(const I2C_Config *config, const uint8_t *data, uint16_t len);
I2C_Result I2C_Read(const I2C_Config *config, uint8_t *data, uint16_t len);
I2C_Result I2C_WriteReg(const I2C_Config *config, uint8_t reg, const uint8_t *data, uint16_t len);
I2C_Result I2C_ReadReg(const I2C_Config *config, uint8_t reg, uint8_t *data, uint16_t len);
bool I2C_CheckDevice(const I2C_Config *config);

#endif /* _I2C_SOFTWARE_H_ */
