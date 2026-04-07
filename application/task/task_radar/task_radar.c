/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_radar.c
 * Desc: Radar module task implementation
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */
#include "task_radar.h"
#include "bsp_radar.h"
#include "msg_protocol.h"
#include "user_parameter.h"
#include "task_system_time.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "task_radar"

// 雷达任务驱动句柄
static radar_task_driver_t g_radar_task_driver;

/**
 * @brief 雷达事件回调函数
 * @param event 雷达事件
 * @param value 雷达事件值
 */
static void radar_task_callback(uint8_t event, uint8_t value)
{
    OB_LOGD(TAG, "[%s] event[%d]  value[%d]", __func__, event, value);
    switch (event)
    {
    case RADAR_EVENT_INIT_OK:
        break;
    case RADAR_EVENT_PAUSE:
        break;
    case RADAR_EVENT_STAY_TRIGGER:
        uart_msg_stay_warn(EVENT_CODE_RADAR_STAY,value);
        break;
    case RADAR_EVENT_WAKE:
        uart_msg_stay_warn(EVENT_CODE_RADAR_WAKE,value);
        break;
    default:
        break;
    }
}

/**
 * @brief 雷达任务唤醒
 * @return 执行结果（0-成功，非0-失败）
 */
uint8_t radar_task_wake(void)
{
    return g_radar_task_driver.io->wake();
}

/**
 * @brief 雷达任务休眠
 * @return NULL
 */
void radar_task_sleep(void)
{
    if (get_user_parameter(PARAMETER_HUMAN_SENSOR_SETTING) == OB_LOCK_MOTION_DETECT_SETTINGS_DISABLE){
        radar_task_handle(RADAR_HANDLE_STAY, Disabled);
        g_radar_task_driver.io->sleep(Disabled);
        return;
    }
    else{
        g_radar_task_driver.io->sleep(Enabled);
    }

    if (get_user_parameter(PARAMETER_LOITER_ALARM) == LOITER_ALARM_FLAG_MIN){
        radar_task_handle(RADAR_HANDLE_STAY, Disabled);
    }
    else{
        radar_task_handle(RADAR_HANDLE_STAY, Enabled);
    }
}


uint8_t radar_task_is_wake(void)
{
    if (get_user_parameter(PARAMETER_HUMAN_SENSOR_SETTING) == OB_LOCK_MOTION_DETECT_SETTINGS_DISABLE)
        return WAKE_SOURCE_NULL;

    if (g_radar_task_driver.io->is_wake())
        return WAKE_SOURCE_RADAR;

    return WAKE_SOURCE_NULL;
}

/**
 * @brief 处理雷达任务使能/禁用
 * @param handle 雷达处理
 * @param enable 使能状态（1-使能，0-禁用）
 */
void radar_task_handle(uint8_t handle, uint8_t enable)
{
    g_radar_task_driver.attribute.enable = enable;
    g_radar_task_driver.io->handle(handle, enable);
}

/**
 * @brief 雷达任务初始化
 */
void radar_task_init(void)
{
    g_radar_task_driver.io = bsp_radar_get_driver(radar_task_callback);
    if (NULL != g_radar_task_driver.io)
    {
        g_radar_task_driver.io->init();
        g_radar_task_driver.io->set_detect(RADAR_TASK_DETECT_DEFAULT);
        g_radar_task_driver.io->handle(RADAR_HANDLE_STAY, Enabled);
    }
    else
    {
        OB_LOGE(TAG, "Err: radar driver is null");
    }
}

/**
 * @brief 雷达任务设置检测参数
 * @param type 检测类型
 */
void radar_task_set_detect(uint8_t type)
{
    g_radar_task_driver.io->set_detect(type);
}

/**
 * @brief 雷达任务暂停
 * @param value 暂停时长（ms），0表示永久暂停
 */
void radar_task_pause(uint32_t value)
{
    g_radar_task_driver.io->pause(value);
}

/**
 * @brief 雷达逗留扫描
 * @param value 
 */
void radar_task_stay_scan(uint32_t value)
{
    g_radar_task_driver.io->stay_scan(value);
}

/**
 * @brief 雷达任务主循环（扫描雷达状态）
 */
void radar_task_loop(void)
{
    g_radar_task_driver.io->loop();
}


/*****************************/
