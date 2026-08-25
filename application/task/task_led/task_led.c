/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_led.c
 * Desc: LED任务驱动封装（初始化/事件处理/状态控制）
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

#include "task_led.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "task_led"

/*************************Variable*************************/
static led_task_driver_t ledTaskDriver = {0}; // 初始化清零，避免野指针

/*************************Function*************************/
/**
 * @brief LED任务回调函数（处理LED事件）
 * @param ledType: LED类型
 * @param ledEvent: LED事件类型
 * @param ledState: LED状态
 * @param ledValue: LED值（0x0000~0xFFFF）
 */
static void ledTaskCallback(uint8_t ledType, uint8_t ledEvent, uint8_t ledState, uint8_t ledValue)
{
    OB_LOGI(TAG, "[%s] ledType=%d,ledEvent=%d,ledState=%d,ledValue=0x%04X",
            __func__, ledType, ledEvent, ledState, ledValue);
    
    switch (ledEvent)
    {
    case LED_EVENT_ALL_ON:
        // 所有LED开启逻辑（待补充）
        break;
    case LED_EVENT_ALL_OFF:
        // 所有LED关闭逻辑（待补充）
        break;
    case LED_EVENT_SET_VALUE:
        // 设置LED值逻辑（待补充）
        break;
    case LED_EVENT_KEY_BOARD:
        // 键盘关联LED逻辑（待补充）
        break;    
    default:
        OB_LOGW(TAG, "[%s] Unsupported ledEvent=%d", __func__, ledEvent);
        break;
    }
}

/**
 * @brief LED任务初始化（获取驱动+初始化硬件）
 */
void ledTaskInit(void)
{
    // 清空驱动结构体，避免残留数据
    memset(&ledTaskDriver, 0, sizeof(led_task_driver_t));
    
    // 获取LED驱动接口
    ledTaskDriver.io = bsp_led_get_driver(ledTaskCallback);
    if (ledTaskDriver.io != NULL)
    {
        // 驱动接口有效性二次校验（避免接口内函数指针为空）
        if (ledTaskDriver.io->init != NULL)
        {
            ledTaskDriver.io->init();
            ledTaskDriver.attribute.enable = true;
            OB_LOGI(TAG, "[%s] LED task init success", __func__);
        }
        else
        {
            OB_LOGE(TAG, "[%s] Err:LED driver init func is null", __func__);
            ledTaskDriver.attribute.enable = false;
        }
    }
    else
    {
        OB_LOGE(TAG, "[%s] Err:led driver is null", __func__);
        ledTaskDriver.attribute.enable = false;
    }
}

/**
 * @brief LED任务事件处理
 * @param event_type: 事件类型
 * @param event_code: 事件码
 */
void ledTaskHandle(uint8_t event_type, uint16_t event_code)
{
    if (ledTaskDriver.attribute.enable)
    {
        if (ledTaskDriver.io != NULL && ledTaskDriver.io->handle != NULL)
        {
            ledTaskDriver.io->handle(event_type, event_code);
        }
        else
        {
            OB_LOGE(TAG, "[%s] Err:LED driver handle func is null", __func__);
        }
    }
    else
    {
        OB_LOGW(TAG, "[%s] LED task is disabled, ignore event", __func__);
    }
}

/**
 * @brief LED任务休眠（关闭LED硬件）
 */
void ledTaskSleep(void)
{
    if (ledTaskDriver.attribute.enable)
    {
        if (ledTaskDriver.io != NULL && ledTaskDriver.io->sleep != NULL)
        {
            ledTaskDriver.io->sleep();
            OB_LOGI(TAG, "[%s] LED task sleep", __func__);
        }
        else
        {
            OB_LOGE(TAG, "[%s] Err:LED driver sleep func is null", __func__);
        }
    }
}

/**
 * @brief LED任务唤醒（重新初始化LED硬件）
 */
void ledTaskWake(void)
{
    if (ledTaskDriver.attribute.enable)
    {
        if (ledTaskDriver.io != NULL && ledTaskDriver.io->init != NULL)
        {
            ledTaskDriver.io->init();
            OB_LOGI(TAG, "[%s] LED task wake", __func__);
        }
        else
        {
            OB_LOGE(TAG, "[%s] Err:LED driver init func is null", __func__);
        }
    }
}

/**
 * @brief LED任务主循环（周期性处理）
 */
void ledTaskLoop(void)
{
    if (ledTaskDriver.attribute.enable)
    {
        if (ledTaskDriver.io != NULL && ledTaskDriver.io->loop != NULL)
        {
            ledTaskDriver.io->loop();
        }
        else
        {
            OB_LOGE(TAG, "[%s] Err:LED driver loop func is null", __func__);
        }
    }
}

