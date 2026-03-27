/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: radar.c
 * Desc: 
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-12
 */

#include "iic_radar_api.h"
#include "io_radar_api.h"
#include "radar_config.h"
#include "radar_chip_config.h"
#include "hal_gpio.h"
#include "delay.h"          // 增加延时头文件（复位需要）

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "radar"

/****************IIC Device*************/

/***************************************/

void radar_init(void)
{
#if (RADAR_CHECK_SEL == RADAR_CHECK_IIC)
    iic_radar_init();
#elif (RADAR_CHECK_SEL == RADAR_CHECK_IO)
    io_radar_init();
#endif
}

void radar_set_out_int(uint8_t enable)
{
    if (enable == Enabled)
        RADAR_INT_PIN_ENABLE();
    else
        RADAR_INT_PIN_DISABLE();
}

void radar_set_distance(uint8_t type)
{
    switch (type)
    {
    case RADAR_HIGH:
#if (RADAR_CHECK_SEL == RADAR_CHECK_IIC)
        set_distance(POWER_SELECT, LS_STATUS, ((RADAR_GAIN << 4) | 0X03), RADAR_HIGH_VALUE);
#elif (RADAR_CHECK_SEL == RADAR_CHECK_IO)

#endif
        break;
    case RADAR_MIDDLE:
#if (RADAR_CHECK_SEL == RADAR_CHECK_IIC)
        set_distance(POWER_SELECT, LS_STATUS, ((RADAR_GAIN << 4) | 0X03), RADAR_MIDDLE_VALUE);
#elif (RADAR_CHECK_SEL == RADAR_CHECK_IO)

#endif
        break;
    case RADAR_LOW:
#if (RADAR_CHECK_SEL == RADAR_CHECK_IIC)
        set_distance(POWER_SELECT, LS_STATUS, ((RADAR_GAIN << 4) | 0X03), RADAR_LOW_VALUE);
#elif (RADAR_CHECK_SEL == RADAR_CHECK_IO)

#endif
        break;
    case RADAR_NULL:
        break;
    default:
        break;
    }
}

