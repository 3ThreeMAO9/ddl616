/**
 * Copyright (c) 2026 GZ-OB, All rights reserved.
 * File name: io_radar_api.c
 * Desc: 
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2026-03-12
 */

#include "io_radar_api.h"
#include "io_radar_driver.h"
#include "radar_config.h"
#include "hal_gpio.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "radar_api"

/****************IIC Device*************/

/***************************************/

void io_radar_init(void)
{
    OB_LOGI(TAG, "[%s] Start radar init", __func__);
    radar_driver_init();
    RADAR_INT_PIN_ENABLE();
    RADAR_INT_CLEAR_INTSTATE();
}
