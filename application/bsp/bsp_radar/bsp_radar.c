/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_radar.c
 * Desc: Radar module BSP layer implementation
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "bsp_radar.h"

/*************************Variable*************************/
// 雷达底层驱动接口实例
const bsp_radar_drive_io_t bsp_radar = {
    .init = module_radar_init,
    .loop = module_radar_loop,
    .set_detect = module_radar_setting_detect,
    .wake = module_radar_wake,
    .is_wake = module_radar_is_wake_API,
    .handle = module_radar_switch,
    .pause = module_radar_pause,
    .stay_scan = module_radar_stay_scan,
    .sleep = module_radar_sleep,
};

//--------------------------------------------------
/**
 * @brief 获取雷达底层驱动接口
 * @param callback 雷达事件回调函数
 * @return 雷达驱动接口指针
 */
const bsp_radar_drive_io_t* bsp_radar_get_driver(bsp_radar_callback_t callback)
{
    radar_event_register_callback(callback);
    return &bsp_radar;
}
