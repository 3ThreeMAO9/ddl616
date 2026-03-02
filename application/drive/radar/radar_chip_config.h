/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: radar_chip_config.h
 * Desc: 
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef RADAR_CHIP_CONFIG_H
#define RADAR_CHIP_CONFIG_H

#include "iic_radar_api.h"
#include "iic_radar_config.h"
#include "target_config.h"

#include "radar_stay.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/************Macro*************/

/************Enum**************/
typedef enum {
    RADAR_NULL = 0,    // 无检测（关闭检测功能）
    RADAR_LOW,         // 低灵敏度
    RADAR_MIDDLE,      // 中灵敏度
    RADAR_HIGH,        // 高灵敏度
    RADAR_DETECT_MAX   // 枚举边界（用于参数合法性检查）
} radar_detect_sensitivity_e;

typedef enum{
    RADAR_HANDLE_NULL = 0,  // 雷达无处理
    RADAR_HANDLE_STAY,      // 雷达逗留处理
    RADAR_HANDLE_MAX        // 枚举边界
} radar_handle_e;

typedef enum {
    RADAR_EVENT_INIT_OK = 0,        // 初始化完成
    RADAR_EVENT_PAUSE = 1,          // 暂停事件
    RADAR_EVENT_STAY_TRIGGER = 2,   // 逗留触发事件
    RADAR_EVENT_WAKE = 3,           // 唤醒事件
    // 可扩展其他事件
} radar_event_e;

/************Struct************/

/********Type Definition*******/

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif  // RADAR_CHIP_CONFIG_H
