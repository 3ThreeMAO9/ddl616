/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: iic_radar_driver.c
 * Desc: AT10LP雷达底层I2C驱动实现
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "iic_radar_config.h"
#include "iic_radar_driver.h"
#include "hal_gpio.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "radar_driver"

 /**
  * @brief 设置雷达I2C SCL引脚电平
  * @param level 电平状态（true-高电平，false-低电平）
  */
 static inline void RADAR_I2C_SetSCL(bool level)
 {
     HAL_GPIO_Write(RADAR_I2C_SCL_GPIO, RADAR_I2C_SCL_PIN, level);
 }

/**
 * @brief 设置雷达I2C SDA引脚电平
 * @param level 电平状态（true-高电平，false-低电平）
 */
static inline void RADAR_I2C_SetSDA(bool level)
{
    HAL_GPIO_Write(RADAR_I2C_SDA_GPIO, RADAR_I2C_SDA_PIN, level);
}

/**
 * @brief 读取雷达I2C SDA引脚电平
 * @return 引脚电平（true-高电平，false-低电平）
 */
static inline bool RADAR_I2C_GetSDA()
{
    return HAL_GPIO_Read(RADAR_I2C_SDA_GPIO, RADAR_I2C_SDA_PIN);
}


// 雷达I2C配置参数
static const I2C_Config radar_i2c_config = {
    .ops = {
        .set_scl = RADAR_I2C_SetSCL,
        .set_sda = RADAR_I2C_SetSDA,
        .get_sda = RADAR_I2C_GetSDA,
        .delay_us = delay_us,
    },
    .slave_addr = IIC_RADAR_IIC_ADDR,       // 雷达从机地址（来自配置头文件）
    .clock_delay_us = I2C_CLK_DELAY_US,     // 0=175KHz，1=125KHz（根据通信稳定性调整）
    .byte_delay_us = I2C_BYTE_DELAY_US,     // 字节间延时（实测10-20us更稳定，建议调整为10）
    .ack_timeout = I2C_ACK_TIME_OUT,        // ACK超时时间（单位：us）
    .retry_times = I2C_RETRY_TIMES,         // I2C通信失败重试次数
};

/**
 * @brief 雷达驱动初始化
 */
void radar_driver_init(void)
{
    // 初始化I2C总线
    I2C_Init(&radar_i2c_config);
    // I2C引脚初始化（上拉）
    RADAR_I2C_SCL_INIT(1);
    RADAR_I2C_SDA_INIT(1);
    // OUT引脚初始化（输入）
    RADAR_INT_INIT(1);
}

/**
 * @brief 雷达驱动关闭
 */
void radar_driver_uninit(void)
{
    RADAR_I2C_SCL_INIT(0);
    RADAR_I2C_SDA_INIT(0);
    RADAR_INT_PULL_INIT(0);
}

/**
 * @brief 写入雷达模块指定寄存器
 * @param reg_addr 寄存器地址
 * @param buffer 待写入数据缓冲区
 * @param length 写入字节数
 * @return true-成功，false-失败
 */
static uint8_t radar_write_register(uint8_t reg_addr, uint8_t *buffer, uint16_t length)
{
    // 入参合法性校验（防止空指针/非法长度）
    if (buffer == NULL || length == 0)
    {
        OB_LOGW(TAG, "Write reg param error: addr[%02X], len[%d]", reg_addr, length);
        return false;
    }

    I2C_Result result = I2C_WriteReg(&radar_i2c_config, reg_addr, buffer, length);
    if (I2C_OK == result)
    {
        return true;
    }

    OB_LOGD(TAG, "Write reg fail: addr[%02X], len[%d], code[%u]", reg_addr, length, result);

    return false;
}

/**
 * @brief 写入AT10LP指定寄存器
 * @param address 寄存器地址
 * @param data 待写入数据缓冲区
 * @return true-成功，false-失败
 */
uint8_t AT10LP_WriteOneByte(uint8_t address, uint8_t data)
{
    return radar_write_register(address, (uint8_t *)(&data), 1);
}



