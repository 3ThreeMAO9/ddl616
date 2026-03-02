/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: i2c_software.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "i2c_software.h"
#include "stddef.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "i2c_software"

/************************Variable************************/

// ------------------------------
// 内部延时函数
static inline void I2C_Delay(const I2C_Config *config, uint32_t us)
{
    if (config && config->ops.delay_us)
    {
        config->ops.delay_us(us);
    }
}

// 初始化I2C总线
void I2C_Init(const I2C_Config *config)
{
    if (config == NULL)
        return;

    // 初始状态：SCL和SDA都为高
    config->ops.set_scl(true);
    config->ops.set_sda(true);
    I2C_Delay(config, 10); // 延时确保总线稳定
}

// 产生起始信号
I2C_Result I2C_Start(const I2C_Config *config)
{
    if (config == NULL)
        return I2C_ERR_PARAM;

    // SDA和SCL初始为高
    config->ops.set_sda(true);
    config->ops.set_scl(true);
    I2C_Delay(config, config->clock_delay_us);

    if (!config->ops.get_sda())
    {
        return I2C_ERR_BUS;
    }

    // 起始条件：SDA在SCL高时变低
    config->ops.set_sda(false);
    I2C_Delay(config, config->clock_delay_us);
    config->ops.set_scl(false);
    I2C_Delay(config, config->clock_delay_us);

    return I2C_OK;
}

// 产生停止信号
void I2C_Stop(const I2C_Config *config)
{
    if (config == NULL)
        return;

    // 确保SCL为低
    config->ops.set_scl(false);
    config->ops.set_sda(false);
    I2C_Delay(config, config->clock_delay_us);

    // 停止条件：SDA在SCL高时变高
    config->ops.set_scl(true);
    I2C_Delay(config, config->clock_delay_us);
    config->ops.set_sda(true);
    I2C_Delay(config, config->clock_delay_us);
}

static bool I2C_ReadAck(const I2C_Config *config)
{
    bool ack;
    uint16_t timeout = config->ack_timeout;

    // 读取ACK
    config->ops.set_sda(true); // 释放SDA线
    I2C_Delay(config, config->clock_delay_us);
    config->ops.set_scl(true);
    I2C_Delay(config, config->clock_delay_us);

    // 检查ACK
    do
    {
        ack = config->ops.get_sda();
        if (!ack)
        {
            break;
        }
    } while (timeout--);

    config->ops.set_scl(false);
    I2C_Delay(config, config->clock_delay_us);
    config->ops.set_sda(true); // 释放SDA线
    I2C_Delay(config, (config->byte_delay_us));

    return ack;
}

// 写入一个字节
I2C_Result I2C_WriteByte(const I2C_Config *config, uint8_t byte)
{
    if (config == NULL)
        return I2C_ERR_PARAM;

    // 发送8位数据
    for (uint8_t i = 0; i < 8; i++)
    {
        config->ops.set_sda(byte & 0x80);
        byte <<= 1;

        I2C_Delay(config, config->clock_delay_us);
        config->ops.set_scl(true);
        I2C_Delay(config, config->clock_delay_us);
        config->ops.set_scl(false);
    }

    return I2C_ReadAck(config) ? I2C_ERR_NACK : I2C_OK;
}

static void I2C_SendAck(const I2C_Config *config, bool ack)
{
    // 发送ACK/NACK
    config->ops.set_sda(!ack);
    I2C_Delay(config, config->clock_delay_us);
    config->ops.set_scl(true);
    I2C_Delay(config, config->clock_delay_us);
    config->ops.set_scl(false);
    config->ops.set_sda(true); // 释放SDA
}

// 读取一个字节
I2C_Result I2C_ReadByte(const I2C_Config *config, uint8_t *byte, bool ack)
{
    if (config == NULL || byte == NULL)
        return I2C_ERR_PARAM;

    uint8_t b = 0;
    config->ops.set_sda(true); // 确保SDA为输入
    I2C_Delay(config, config->clock_delay_us);

    for (uint8_t i = 0; i < 8; i++)
    {
        b <<= 1;
        config->ops.set_scl(true);
        I2C_Delay(config, config->clock_delay_us);

        if (config->ops.get_sda())
            b |= 1;
        I2C_Delay(config, config->clock_delay_us);

        config->ops.set_scl(false);
        I2C_Delay(config, config->clock_delay_us);
    }

    I2C_SendAck(config, ack);
    *byte = b;
    return I2C_OK;
}

// 写入数据
I2C_Result I2C_Write(const I2C_Config *config, const uint8_t *buffer, uint16_t len)
{
    I2C_Result res;
    if (config == NULL || buffer == NULL || len == 0)
        return I2C_ERR_PARAM;

    uint8_t retry_times = ((config->retry_times) ? (config->retry_times & 0x0F) : 0);
    do
    {
        res = I2C_Start(config);
        if (res == I2C_OK)
        {
            res = I2C_WriteByte(config, (config->slave_addr << 1));
            if (res == I2C_OK)
            {
                for (uint16_t i = 0; i < len; i++)
                {
                    res = I2C_WriteByte(config, buffer[i]);
                    if (res != I2C_OK)
                        break;
                }
            }
        }
        I2C_Stop(config);

        if (res == I2C_OK)
            break;
        delay_us(100);
    } while (retry_times--);

    return res;
}

// 读取数据
I2C_Result I2C_Read(const I2C_Config *config, uint8_t *buffer, uint16_t len)
{
    I2C_Result res;
    if (config == NULL || buffer == NULL || len == 0)
        return I2C_ERR_PARAM;

    uint8_t retry_times = ((config->retry_times) ? (config->retry_times & 0x0F) : 0);
    do
    {
        res = I2C_Start(config);
        if (res == I2C_OK)
        {
            res = I2C_WriteByte(config, (config->slave_addr << 1) | 0x01);
            if (res == I2C_OK)
            {
                for (uint16_t i = 0; i < len; i++)
                {
                    res = I2C_ReadByte(config, &buffer[i], i != (len - 1));
                    if (res != I2C_OK)
                        break;
                }
            }
        }
        I2C_Stop(config);

        if (res == I2C_OK)
            break;
        delay_us(100);
    } while (retry_times--);

    return res;
}

// 写入寄存器
I2C_Result I2C_WriteReg(const I2C_Config *config, uint8_t reg, const uint8_t *buffer, uint16_t len)
{
    I2C_Result res;
    if (config == NULL || buffer == NULL || len == 0)
        return I2C_ERR_PARAM;

    uint8_t retry_times = ((config->retry_times) ? (config->retry_times & 0x0F) : 0);
    do
    {
        res = I2C_Start(config);
        if (res == I2C_OK)
        {
            // 先写设备地址(写模式)
            res = I2C_WriteByte(config, (config->slave_addr << 1));
            if (res == I2C_OK)
            {
                // 写寄存器地址
                res = I2C_WriteByte(config, reg);
                if (res == I2C_OK)
                {
                    // 写数据
                    for (uint16_t i = 0; i < len; i++)
                    {
                        res = I2C_WriteByte(config, buffer[i]);
                        if (res != I2C_OK)
                            break;
                    }
                }
            }
        }
        I2C_Stop(config);

        if (res == I2C_OK)
            break;
        delay_us(100);
    } while (retry_times--);

    return res;
}

// 读取寄存器
I2C_Result I2C_ReadReg(const I2C_Config *config, uint8_t reg, uint8_t *buffer, uint16_t len)
{
    I2C_Result res;
    if (config == NULL || buffer == NULL || len == 0)
        return I2C_ERR_PARAM;

    uint8_t retry_times = ((config->retry_times) ? (config->retry_times & 0x0F) : 0);
    do
    {
        // 先写入寄存器地址
        res = I2C_Start(config);
        if (res == I2C_OK)
        {
            res = I2C_WriteByte(config, config->slave_addr << 1);
            if (res == I2C_OK)
            {
                res = I2C_WriteByte(config, reg);
                if (res == I2C_OK)
                {
                    // 然后读取数据
                    res = I2C_Start(config);
                    if (res == I2C_OK)
                    {
                        res = I2C_WriteByte(config, (config->slave_addr << 1) | 0x01);
                        if (res == I2C_OK)
                        {
                            for (uint16_t i = 0; i < len; i++)
                            {
                                res = I2C_ReadByte(config, &buffer[i], i != (len - 1));
                                if (res != I2C_OK)
                                    break;
                            }
                        }
                    }
                }
            }
        }
        I2C_Stop(config);

        if (res == I2C_OK)
        {
            break;
        }
        delay_us(100);
    } while (retry_times--);

    return res;
}

// 检查设备是否存在
bool I2C_CheckDevice(const I2C_Config *config)
{
    if (config == NULL)
        return I2C_ERR_PARAM;

    bool ack = false;
    if (I2C_Start(config) == I2C_OK)
    {
        ack = I2C_WriteByte(config, config->slave_addr << 1);
    }
    I2C_Stop(config);

    return ack;
}
