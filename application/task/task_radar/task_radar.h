/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_radar.h
 * Desc: Radar module task header file
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */
#ifndef TASK_RADAR_H
#define TASK_RADAR_H

#include "config.h"
#include "radar_chip_config.h"
#include "bsp_radar.h"

// 雷达任务回调函数类型
typedef bsp_radar_callback_t task_radar_callback_t;

/***********Macro***********/
#define RADAR_TASK_DETECT_DEFAULT       RADAR_MIDDLE

/***********Enum***********/

/***********Struct***********/
/**
 * @brief 雷达任务属性结构体
 */
typedef struct{
    uint8_t enable;  // 雷达类型使能数组
}radar_task_attribute_t;

/**
 * @brief 雷达任务驱动句柄结构体
 */
typedef struct{
    const bsp_radar_drive_io_t *io;          // 雷达底层驱动接口
    radar_task_attribute_t attribute;        // 雷达任务属性

}radar_task_driver_t;

/***********Variable***********/

/***********Function***********/
/**
 * @brief 雷达任务初始化
 */
void radar_task_init(void);

/**
 * @brief 雷达任务主循环（扫描雷达状态）
 */
void radar_task_loop(void);

/**
 * @brief 雷达任务唤醒
 * @return 执行结果（0-成功，非0-失败）
 */
uint8_t radar_task_wake(void);

uint8_t radar_task_is_wake(void);

/**
 * @brief 处理雷达任务使能/禁用
 * @param handle 雷达处理
 * @param enable 使能状态（1-使能，0-禁用）
 */
void radar_task_handle(uint8_t handle, uint8_t enable);

/**
 * @brief 检查雷达任务是否忙碌
 * @param type 雷达类型
 * @return 1-忙碌，0-空闲
 */
uint8_t radar_task_is_busy(uint8_t type);

/**
 * @brief 雷达任务暂停
 * @param value 暂停时长（ms），0表示永久暂停
 */
void radar_task_pause(uint32_t value);

/**
 * @brief 雷达逗留扫描
 * @param value 
 */
void radar_task_stay_scan(uint32_t value);

/*****************************/

#endif // TASK_RADAR_H
