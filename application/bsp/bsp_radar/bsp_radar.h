/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_radar.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef BSP_RADAR__HH
#define BSP_RADAR__HH

#include "config.h"
#include "module_radar.h"

// 雷达事件回调函数类型重定义
typedef radar_event_callback_t bsp_radar_callback_t;

/*************************Macro*************************/

/*************************Enum*************************/

/*************************Struct*************************/
/**
 * @brief 雷达底层驱动接口结构体
 */
typedef struct {
    uint8_t (*init)(void);
    void (*loop)(void);
    void (*set_detect)(uint8_t);
    uint8_t (*wake)(void);
    uint8_t (*is_wake)(void);
    void (*handle)(uint8_t, uint8_t);
    void (*pause)(uint32_t);
    void (*stay_scan)(uint32_t);
    void (*sleep)(uint8_t);
} bsp_radar_drive_io_t;

/*************************Variable*************************/

/*************************Function*************************/
/**
 * @brief 获取雷达底层驱动接口
 * @param callback 雷达事件回调函数指针
 * @return 雷达驱动接口结构体指针
 */
const bsp_radar_drive_io_t* bsp_radar_get_driver(bsp_radar_callback_t callback);

/**********************************************************/

#endif
