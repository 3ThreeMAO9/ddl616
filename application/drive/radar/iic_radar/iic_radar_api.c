/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: iic_radar_api.c
 * Desc: AT10LP雷达模块高层API实现
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "iic_radar_api.h"
#include "iic_radar_driver.h"
#include "iic_radar_config.h"
#include "target_config.h"  // 修正拼写错误：target_conifg.h → target_config.h
#include "hal_gpio.h"
#include "delay.h"          // 增加延时头文件（复位需要）

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "radar_api"

/****************IIC Device*************/
// 雷达模块运行状态句柄
static radar_handle_t radar_handle = {0};

/***************************************/


/***********雷达核心配置API***********/
/**
 * @brief 设置窗口检测参数
 * @param window_length 检测窗口长度
 * @param window_threshold 检测窗口阈值
 */
void radar_set_window_detect(uint8_t window_length, uint8_t window_threshold)
{
    OB_LOGD(TAG, "Set window detect: len=%d, threshold=%d", window_length, window_threshold);
    AT10LP_WriteOneByte(0x30, window_length);
    AT10LP_WriteOneByte(0x31, window_threshold);
}

/**
 * @brief 设置重复触发和输出反相
 * @param outRepeatTrigger 重复触发：0-开启，1-关闭
 * @param outInverse 输出反相：0-高电平，1-低电平
 */
void radar_set_outReptTrigger_outInverse(uint8_t outRepeatTrigger, uint8_t outInverse)
{
    OB_LOGD(TAG, "Set output: repeat=%d, inverse=%d", outRepeatTrigger, outInverse);
    AT10LP_WriteOneByte(0x32, (uint8_t)((outRepeatTrigger << 6) | (outInverse << 5) | 0x04));
}

/**
 * @brief 雷达软件复位
 */
void radar_soft_reset(void)
{
    OB_LOGI(TAG, "Radar soft reset");
    AT10LP_WriteOneByte(0x00, 0x00);
    AT10LP_WriteOneByte(0x00, 0x01);
}

/**
 * @brief 设置雷达芯片功耗模式和光敏使能
 * @param power_select 功耗选择（POWER_26MA等宏定义）
 * @param ls_onoff 光敏使能：1-关闭，0-开启
 */
void set_chip_power(unsigned char power_select, unsigned char ls_onoff)
{
    if(power_select == POWER_26MA) {
        AT10LP_WriteOneByte(0x6c, 0x43 | (ls_onoff << 4));
        AT10LP_WriteOneByte(0x6e, 0x98 | 0x40);
        return;
    }

    AT10LP_WriteOneByte(0x6c, 0xC3 | (ls_onoff << 4));

    AT10LP_WriteOneByte(0x6d, 0xf0 | 0x08 | 0x00);
    AT10LP_WriteOneByte(0x6e, 0x80 | 0x40 | 0x20 | (0x01 << 3));
}

/**
 * @brief 设置雷达芯片TIA增益
 * @param tai_gain TIA增益值（对应寄存器0x60的配置值）
 * @note 写入成功时会设置radar_existence_flag为true，用于芯片检测
 */
void set_tia_gain(unsigned char tai_gain)
{
    if(true == AT10LP_WriteOneByte(0x60, tai_gain))
        radar_handle.existence_flag = true;

    AT10LP_WriteOneByte(0x69, 0xbe);
}

/**
 * @brief 设置雷达检测距离相关的delta参数
 * @param delta 距离参数值（32位无符号整数）
 * @note 写入成功时会设置radar_existence_flag为true，用于芯片检测
 */
void set_delta(unsigned int delta)
{
    if(true == AT10LP_WriteOneByte(0x0f, (unsigned char)(delta)))
        radar_handle.existence_flag = true;

    AT10LP_WriteOneByte(0x10, (unsigned char)(delta >> 8));
}

/**
 * @brief 一站式设置雷达检测距离相关参数
 * @param power_select 功耗选择（POWER_26MA等宏定义）
 * @param ls_onoff 光敏使能：1-关闭，0-开启
 * @param gain TIA增益值
 * @param delta 距离参数值（32位无符号整数）
 * @note 该函数会依次调用set_chip_power、set_tia_gain、set_delta函数
 */
void set_distance(unsigned char power_select, unsigned char ls_onoff, unsigned char gain, unsigned int delta)
{
    set_chip_power(power_select, ls_onoff);
    set_tia_gain(gain);
    set_delta(delta);
}

/**
 * @brief 设置灯光开启基准时间
 * @param light_on_base_time 基准时间（32位，ms）
 */
void radar_set_light_on_base_time(uint32_t light_on_base_time)
{
    OB_LOGD(TAG, "Set light base time: %lu ms", light_on_base_time);
    AT10LP_WriteOneByte(0x41, (uint8_t)(light_on_base_time));
    AT10LP_WriteOneByte(0x42, (uint8_t)(light_on_base_time >> 8));
    AT10LP_WriteOneByte(0x43, (uint8_t)(light_on_base_time >> 16));
    AT10LP_WriteOneByte(0x44, (uint8_t)(light_on_base_time >> 24));
}

/**
 * @brief 设置灯光开启时间
 * @param light_on_time 开启时间（32位，ms）
 */
void radar_set_light_on_time(uint32_t light_on_time)
{
    OB_LOGD(TAG, "Set light on time: %lu ms", light_on_time);
    AT10LP_WriteOneByte(0x45, 0x01);
    AT10LP_WriteOneByte(0x46, (uint8_t)(light_on_time));
    AT10LP_WriteOneByte(0x47, (uint8_t)(light_on_time >> 8));
    AT10LP_WriteOneByte(0x48, (uint8_t)(light_on_time >> 16));
    AT10LP_WriteOneByte(0x49, (uint8_t)(light_on_time >> 24));
}

/**
 * @brief 设置休眠时间
 * @param sleep_time 休眠时间（32位，ms）
 */
void radar_set_sleep_time(uint32_t sleep_time)
{
    OB_LOGD(TAG, "Set sleep time: %lu ms", sleep_time);
    AT10LP_WriteOneByte(0x4a, (uint8_t)(sleep_time));
    AT10LP_WriteOneByte(0x4b, (uint8_t)(sleep_time >> 8));
    AT10LP_WriteOneByte(0x4c, (uint8_t)(sleep_time >> 16));
    AT10LP_WriteOneByte(0x4d, (uint8_t)(sleep_time >> 24));
}

/**
 * @brief 设置灯光关闭保护时间
 * @param light_off_time 关闭时间（16位，ms）
 */
void radar_set_light_off_time(uint16_t light_off_time)
{
    OB_LOGD(TAG, "Set light off time: %u ms", light_off_time);
    AT10LP_WriteOneByte(0x52, (uint8_t)(light_off_time));
    AT10LP_WriteOneByte(0x53, (uint8_t)(light_off_time >> 8));
}

/**
 * @brief 设置光敏阈值
 * @param chip_power_select 功耗模式（POWER_xxx）
 * @param onoff 光敏使能：1-关闭，0-开启
 * @param ls_value_low 光敏低值阈值（0~1023）
 * @param ls_value_high 光敏高值阈值（0~1023）
 */
void radar_set_light_sensor_threshold(uint8_t chip_power_select, uint8_t onoff, 
                                     uint16_t ls_value_low, uint16_t ls_value_high)
{
    OB_LOGD(TAG, "Set light sensor: power=%d, onoff=%d, low=%d, high=%d",
            chip_power_select, onoff, ls_value_low, ls_value_high);
    
    // 功耗模式适配
    if(chip_power_select == POWER_26MA) {
        AT10LP_WriteOneByte(0x6c, 0x43 | (onoff << 4));
    } else {
        AT10LP_WriteOneByte(0x6c, 0xC3 | (onoff << 4));
    }

    // 光敏阈值配置
    AT10LP_WriteOneByte(0x33, (uint8_t)(ls_value_low));
    AT10LP_WriteOneByte(0x34, (uint8_t)(ls_value_high));
    AT10LP_WriteOneByte(0x35, (uint8_t)((ls_value_low >> 8) | ((ls_value_high >> 8) << 2)));
}

/**
 * @brief AT10LP基础参数初始化（从配置文件加载参数）
 * @note 根据编译宏定义选择不同的配置分支，加载窗口检测、输出模式、灯光时间、光敏阈值等参数
 */
void at10lp_base_init(void)
{
    OB_LOGI(TAG, "AT10LP base config init");

    // 窗口检测配置
#ifdef FUNC_WIN
    radar_set_window_detect(WINDOW_LENGTH, WINDOW_THRESHOLD);
#endif

    // 感应顺延&输出反相配置
#ifdef FUC_SET_DETECT_INVERSE
    radar_set_outReptTrigger_outInverse(LIGHT_ON_DISABLE_DETECT, OUTPUT_INVERSE);
#if LIGHT_ON_DISABLE_DETECT
    radar_set_light_on_base_time(POWER_DET_TIME);
    radar_set_light_on_time(DELAY_TIME);
    radar_set_sleep_time(SLEEP_TIME);
#else
    radar_set_light_off_time(LIGHT_OFF_TIME);
    radar_set_light_on_base_time(LIGHT_ON_BASE_TIME);
    radar_set_light_on_time(LIGHT_ON_TIME);
#endif
#else
    // 默认配置
    radar_set_light_off_time(LIGHT_OFF_TIME);
    radar_set_light_on_base_time(LIGHT_ON_BASE_TIME);
    radar_set_light_on_time(LIGHT_ON_TIME);
#endif

    // 光敏阈值配置
    radar_set_light_sensor_threshold(POWER_SELECT, LS_STATUS, LS_VALUE_LOW, LS_VALUE_HIGH);
}

/**
 * @brief 雷达模块顶层初始化函数
 * @note 整合驱动初始化和参数配置，对外提供统一入口
 * @note 包含状态句柄初始化、底层驱动初始化、软件复位、基础参数加载等步骤
 * @note 具备防重复初始化逻辑
 */
void iic_radar_init(void)
{
    OB_LOGI(TAG, "[%s] Start radar init", __func__);

    // 避免重复初始化
    if (radar_handle.init_flag)
    {
        OB_LOGW(TAG, "Radar already initialized");
        return;
    }

    // 1. 初始化状态句柄
    radar_handle.init_flag = true;
    radar_handle.error_cnt = 0;
    radar_handle.reset_period = 0;

    // 2. 底层驱动初始化（I2C+GPIO）
    radar_driver_init();

    // 3. 软件复位（确保芯片初始状态）
    radar_soft_reset();

    // 4. 加载基础配置参数
    at10lp_base_init();

    set_distance(POWER_SELECT, LS_STATUS, ((RADAR_GAIN << 4) | 0X03), RADAR_LOW_VALUE);

    if(radar_handle.existence_flag == false){
        radar_handle.init_flag = false;
        radar_driver_uninit();
        OB_LOGE(TAG, "[%s] radar_handle.existence_flag = null", __func__);
    }

    OB_LOGI(TAG, "[%s] Radar init completed", __func__);
}
