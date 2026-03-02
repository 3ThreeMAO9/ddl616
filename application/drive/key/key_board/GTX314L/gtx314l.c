/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: gtx314l.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "gtx314l.h"
#include "gtx314l_config.h"
#include "i2c_software.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "GTX314L"

/****************IIC Device*************/
static const uint16_t touch_value_tab[] = KEY_VALUE_TAB;
static touch_handle_t touch_handle = {0};

/***************************************/

static inline void TOUCH_I2C_SetSCL(bool level)
{
    HAL_GPIO_Write(TOUCH_I2C_SCL_GPIO, TOUCH_I2C_SCL_PIN, level);
}

static inline void TOUCH_I2C_SetSDA(bool level)
{
    HAL_GPIO_Write(TOUCH_I2C_SDA_GPIO, TOUCH_I2C_SDA_PIN, level);
}

static inline bool TOUCH_I2C_GetSDA()
{
    return HAL_GPIO_Read(TOUCH_I2C_SDA_GPIO, TOUCH_I2C_SDA_PIN);
}

static const I2C_Config touch_i2c_config = {
    .ops = {
        .set_scl = TOUCH_I2C_SetSCL,
        .set_sda = TOUCH_I2C_SetSDA,
        .get_sda = TOUCH_I2C_GetSDA,
        .delay_us = delay_us,
    },
    .slave_addr = GTX314L_IIC_ADDR,
    .clock_delay_us = 0, // =1，实测频率约125KHz左右（=0，实测175KHz，未压测通信稳定性）
    .byte_delay_us = 5,  // 实测字节间延时10-20us之间
    .ack_timeout = 100,
    .retry_times = 2, // 失败允许重传2次
};

static uint8_t touch_write_register(uint8_t reg_addr, uint8_t *buffer, uint16_t lenth)
{
    I2C_Result result;

    result = I2C_WriteReg(&touch_i2c_config, reg_addr, buffer, lenth);
    if (I2C_OK == result)
    {
        return true;
    }

    OB_LOGD(TAG, "Fail[%s]: code[%u]", "I2C write packet", result);

    return false;
}

static uint8_t touch_read_register(uint8_t reg_addr, uint8_t *buffer, uint16_t lenth)
{
    I2C_Result result;

    result = I2C_ReadReg(&touch_i2c_config, reg_addr, buffer, lenth);
    if (I2C_OK == result)
    {
        return true;
    }

    OB_LOGD(TAG, "Fail[%s]: code[%u]", "I2C write packet", result);
    return false;
}

static uint8_t touch_write_register_value(uint8_t reg_addr, const uint8_t value)
{
    return touch_write_register(reg_addr, (uint8_t *)(&value), 1);
}

static uint8_t check_touch_ready(void)
{
    if (I2C_OK == I2C_CheckDevice(&touch_i2c_config))
    {
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
static uint8_t touch_write_parameter(uint8_t *sensity)
{
    const touch_register_t reg_tab[] = {
        {0x10, 0x10},               //中断输出脉冲模式，单件模式
        {0x14, 0x00},               //按键扫描时间11ms左右
        {0x19, 0x01},               //开启滑动模式
        {0x66, 0x02},
        {0x40, 0x02},
        {0x41, 0x01},
        {0x42, 0x01},
        {0x43, 0x00},
        {0x44, 0x02},
        {0x45, 0x03},
        {0x46, 0x00},
        {0x47, 0x02},
        {0x13, 0x05},
        {0x0A, 0x30},
    };

    for (uint8_t i = 0; i < (sizeof(reg_tab) / sizeof(touch_register_t)); i++)
    {
        if (!touch_write_register_value(reg_tab[i].reg, reg_tab[i].value))
        {
            OB_LOGD(TAG, "Fail: touch write parameter[%u]", i);
            return false;
        }
    }

    return touch_write_register(0x20, sensity, TOUCH_CH_CNT);
}

static uint8_t touch_reset_deal(uint8_t hard_reset)
{
    const touch_register_t reg_tab[] = {
        {0x0B, 0x01},
        {0x0B, 0x00},
    };

    if (hard_reset)
    {
        SET_TOUCH_RESET(0);
        delay_ms(1);
        SET_TOUCH_RESET(1);
    }
    else
    {
        for (uint8_t i = 0; i < (sizeof(reg_tab) / sizeof(touch_register_t)); i++)
        {
            if (!touch_write_register_value(reg_tab[i].reg, reg_tab[i].value))
            {
                OB_LOGD(TAG, "Fail: touch soft reset[%u]", i);
                return false;
            }
            delay_us(30);
        }
    }

    uint8_t time_out = 100;
    do
    {
        delay_ms(1);
        if (check_touch_ready())
        {
            return true;
        }
    } while (time_out--);

    OB_LOGD(TAG, "Fail: touch reset[%u]", hard_reset);
    return false;
}

static uint8_t touch_reset_renew_state(uint8_t hard_reset)
{
    if (!touch_reset_deal(hard_reset))
    {
        return false;
    }
    return touch_write_parameter(touch_handle.sensity);
}

uint8_t touch_set_sensity(uint8_t *sensity)
{
    if (compare_arrays(touch_handle.sensity, sensity, TOUCH_IC_CH_CNT))
    {
        return true;
    }
    memcpy((uint8_t *)(&touch_handle.sensity), sensity, TOUCH_IC_CH_CNT);
    return touch_reset_renew_state(false);
}

uint8_t touch_init(uint8_t *sensity)
{
    OB_LOGD(TAG, "[%s]", __func__);
    if (touch_handle.init)
    {
        return true;
    }
    touch_handle.init = true;
    touch_handle.error_cnt = 0;
    touch_handle.reset_period = 0;
    memcpy((uint8_t *)(touch_handle.sensity), sensity, TOUCH_CH_CNT);

    // I2C init
    I2C_Init(&touch_i2c_config);

    TOUCH_RESET_INIT(0);
    TOUCH_WAKE_INIT(1);
    TOUCH_I2C_SCL_INIT(1);
    TOUCH_I2C_SDA_INIT(1);

    return touch_reset_renew_state(true);
}

uint8_t touch_sleep_config(void)
{
    const touch_register_t reg_tab[] = {
        {0x14, 0x02},
        {0x19, 0x00},
    };

    TOUCH_WAKE_PIN_ENABLE();
    HAL_GPIO_ClearIntState(TOUCH_WAKE_GPIO, TOUCH_WAKE_PIN);

    for (uint8_t i = 0; i < (sizeof(reg_tab) / sizeof(touch_register_t)); i++)
    {
        if (!touch_write_register_value(reg_tab[i].reg, reg_tab[i].value))
        {
            OB_LOGD(TAG, "Fail: touch sleep[%u]", i);
            break;
        }
    }

    return true;
}

uint8_t touch_wake_config(uint8_t touch_wake)
{
    const touch_register_t reg_tab[] = {
        {0x14, 0x00},
        {0x19, 0x01},
    };

    TOUCH_WAKE_PIN_DISABLE();
    TOUCH_WAKE_CLEAR_INTSTATE();

    if (touch_wake)
    {
        for (uint8_t i = 0; i < (sizeof(reg_tab) / sizeof(touch_register_t)); i++)
        {
            if (!touch_write_register_value(reg_tab[i].reg, reg_tab[i].value))
            {
                OB_LOGD(TAG, "Fail: touch wake[%u]", i);
                return false;
            }
        }
        return true;
    }

    return touch_reset_renew_state(true);
}

static key_value_t touch_convert_value(const uint16_t key_board_result)
{
    uint8_t i;
    uint16_t key_board_ch;

    key_board_ch = key_board_result;
    
    for (i = 0; i < TOUCH_IC_CH_CNT; i++)
    {
        if (key_board_ch & 0x0001)
        {
            return ((key_value_t)touch_value_tab[i]);
        }
        key_board_ch >>= 1;
    }

    return KEY_NULL;
}

static uint8_t touch_scan_reset(const uint8_t scan_period)
{
    uint8_t monitor_flag;
    if (touch_handle.error_cnt < TOUCH_IIC_ERROR_CNT_MAX)
    {
        if (touch_handle.reset_period >= scan_period)
        {
            touch_handle.reset_period = 0;

            if (!touch_read_register(0x0A, (uint8_t *)(&monitor_flag), 1))
            {
                return false;
            }
            if (!(0x01 & monitor_flag))
            {
                return false;
            }
        }
        else
        {
            touch_handle.reset_period++;
            return false;
        }
    }

    touch_handle.reset_period = 0;
    touch_handle.error_cnt = 0;
    touch_reset_renew_state(true);

    return true;
}

uint8_t touch_scan(uint8_t *touch_value)
{
    uint16_t temp;

    if (!touch_scan_reset(TOUCH_RESET_SCAN_PERIOD))
    {
        if (touch_read_register(0x02, (uint8_t *)(&temp), 2))
        {
            touch_handle.error_cnt = 0;
            *touch_value = touch_convert_value(temp);
            return true;
        }
        else
        {
            touch_handle.error_cnt++;
        }
    }

    return false;
}

uint8_t is_touch_wake(void)
{
    if (TOUCH_WAKE_READ_INTSTATE())
    {
        TOUCH_WAKE_CLEAR_INTSTATE();

        if (!READ_TOUCH_WAKE())
        {
            return true;
        }
    }

    return false;
}
