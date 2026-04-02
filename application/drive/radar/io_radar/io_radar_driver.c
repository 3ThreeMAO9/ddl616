/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: io_radar_driver.c
 * Desc: 
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-12
 */

#include "io_radar_driver.h"
#include "radar_config.h"
#include "hal_gpio.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "radar_api"

/****************IIC Device*************/

/***************************************/

/**
 * @brief 雷达驱动初始化
 */
void radar_driver_init(void)
{
    // OUT引脚初始化（输入）
    // RADAR_INT_INIT(1);

    // OUT引脚上拉判断是否存在雷达-----如果电平被拉低 说明有雷达，则后续修改成输入模式
    RADAR_INT_PULLUP_INIT(1);
    // I2C引脚初始化（上拉）
    RADAR_PB2_SCL_INIT(1);
    RADAR_PB3_SDA_INIT(1);

}

