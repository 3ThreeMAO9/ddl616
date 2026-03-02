/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: module_radar.h
 * Desc: Radar module radar header file
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef MODULE_RADAR_H
#define MODULE_RADAR_H

#include "config.h"

/************Macro*************/

/************Enum**************/


/************Struct************/

/********Type Definition*******/
typedef void (*radar_event_callback_t)(uint8_t, uint8_t);

/***********Variable***********/

/***********Function***********/
/**
 * @brief 雷达模块初始化
 * @return 初始化结果（true-成功，false-失败）
 */
uint8_t module_radar_init(void);

/**
 * @brief 雷达模块扫描主循环
 */
void module_radar_loop(void);

/**
 * @brief 雷达模块唤醒
 * @return 唤醒结果（true-成功，false-失败）
 */
uint8_t module_radar_wake(void);

/**
 * @brief 雷达使能/禁用切换
 * @param handle 雷达处理
 * @param flag 使能标志（true-使能，false-禁用）
 */
void module_radar_switch(uint8_t handle, uint8_t flag);

/**
 * @brief 雷达模块暂停
 * @param value 暂停时长（ms）
 */
void module_radar_pause(uint32_t value);

/**
 * @brief 设置雷达检测灵敏度
 * @param type 灵敏度类型（radar_detect_sensitivity_e枚举值）
 * @note 不同灵敏度对应不同的检测距离/响应阈值
 */
void module_radar_setting_detect(uint8_t type);

/**
 * @brief 注册雷达回调函数
 * @param callback 回调函数指针
 */
void radar_event_register_callback(radar_event_callback_t callback);

/**
 * @brief 雷达模块逗留扫描
 * @param value 
 */
void module_radar_stay_scan(uint32_t value);

/*****************************/

#endif // MODULE_RADAR_H
