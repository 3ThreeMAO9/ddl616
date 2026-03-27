/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: iic_radar_api.c
 * Desc: AT10LP雷达模块高层API实现
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "hal_timer.h"
#include "system_timer.h"
#include "module_radar.h"
#include "radar_chip_config.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "radar"

/*************************Variable*************************/
static radar_event_callback_t radar_callback;
static uint32_t radar_wake_tick = 0;
// ---------------------------------------------------------
// @brief 雷达事件回调
// @param radarType，雷达类型
// @param radarEvent，雷达事件
// @return true - sucess; false - fail
// ---------------------------------------------------------
static void radarEvent_callback(uint8_t radarType, uint8_t radarEvent)
{
    if (NULL != radar_callback)
    {
        radar_callback(radarType, radarEvent);
    }
}

// @brief 注册雷达回调函数
// @param callback 回调函数指针
void radar_event_register_callback(radar_event_callback_t callback)
{
    radar_callback = callback;
}


/**
 * @brief 雷达模块初始化
 * @return 初始化结果（true-成功，false-失败）
 */
uint8_t module_radar_init(void)
{
    OB_LOGD(TAG, "[%s]", __func__);
    radar_init();
    radarEvent_callback(RADAR_EVENT_INIT_OK, NULL);
    return true;
}

/**
 * @brief 雷达使能/禁用切换
 * @param handle 雷达处理
 * @param flag 使能标志（true-使能，false-禁用）
 */
void module_radar_switch(uint8_t handle, uint8_t flag)
{
    // 1. 参数合法性校验：仅处理逗留检测类型
    if (RADAR_HANDLE_STAY != handle) {
        OB_LOGW(TAG, "Unsupported radar type: %d, only RADAR_HANDLE_STAY is valid", handle);
        return;
    }

    // 2. 校验flag值，避免非法参数
    if (flag != Enabled && flag != Disabled) {
        OB_LOGW(TAG, "Invalid radar flag: %d, must be Enabled(%d) or Disabled(%d)", 
                flag, Enabled, Disabled);
        return;
    }

    // 3. 调用底层接口设置逗留检测使能
    radar_stay_set_enable((flag == Enabled) ? true : false);
}

/**
 * @brief 设置雷达检测灵敏度
 * @param type 灵敏度类型（radar_detect_sensitivity_e枚举值）
 * @note 不同灵敏度对应不同的检测距离/响应阈值
 */
void module_radar_setting_detect(uint8_t type)
{
    switch (type)
    {
    case RADAR_HIGH:
        radar_set_distance(RADAR_HIGH);
        break;
    case RADAR_MIDDLE:
        radar_set_distance(RADAR_MIDDLE);
        break;
    case RADAR_LOW:
        radar_set_distance(RADAR_LOW);
        break;
    case RADAR_NULL:
        break;
    default:
        break;
    }
}



/**
 * @brief 雷达模块暂停
 * @param value 暂停时长（ms）
 */
void module_radar_pause(uint32_t value)
{
    radarEvent_callback(RADAR_EVENT_PAUSE, NULL);
}

/**
 * @brief 雷达模块逗留扫描
 * @param value 
 */
void module_radar_stay_scan(uint32_t value)
{
    radar_stay_scan();
}


/**
 * @brief 雷达模块唤醒
 * @return 唤醒结果（true-成功，false-失败）
 */
uint8_t module_radar_wake(void)
{
    radarEvent_callback(RADAR_EVENT_WAKE, NULL);
    radar_wake_tick = system_inc_time_cnt(5000);
    return false;
}

/**
 * @brief 雷达触发判断
 * @return 唤醒结果（true-成功，false-失败）
 */
uint8_t module_radar_is_wake_API(void)
{
    if (RADAR_INT_READ_INTSTATE()) {
        RADAR_INT_CLEAR_INTSTATE();

        if (READ_RADAR_INT()) {
            return true;
        }
    }

    return false;
}


/**
 * @brief 雷达模块扫描主循环
 */
void module_radar_loop(void)
{
    radar_stay_state_e stay_state = radar_stay_get_state();
    if (stay_state == RADAR_STAY_STATE_TRIGGERED) {
        // 检测到逗留触发
        radarEvent_callback(RADAR_EVENT_STAY_TRIGGER, NULL);
        radar_stay_reset();
    }

    // 每次触发最短5s
    if (system_out_time_cnt(radar_wake_tick))
    {
        if (true == module_radar_is_wake_API())
        {
            module_radar_wake();
        }
    }
}
