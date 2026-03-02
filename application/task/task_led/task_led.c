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
 * @brief 获取LED任务使能状态
 * @retval 1-使能，0-禁用
 */
uint8_t ledTaskIsEnable(void)
{
    return ledTaskDriver.attribute.enable;
}

/**
 * @brief 设置LED任务使能状态
 * @param enable: 1-使能，0-禁用
 */
void ledTaskSetEnable(uint8_t enable)
{
    ledTaskDriver.attribute.enable = (enable != 0) ? 1 : 0; // 归一化状态
    OB_LOGI(TAG, "[%s] LED task enable set to %d", __func__, ledTaskDriver.attribute.enable);
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
 * @brief 设置LED值
 * @param value: 要设置的LED值（0x0000~0xFFFF）
 */
void ledtaskSetValue(uint16_t value)
{
    if (ledTaskDriver.attribute.enable)
    {
        if (ledTaskDriver.io != NULL && ledTaskDriver.io->set_value != NULL)
        {
            ledTaskDriver.io->set_value(value);
            OB_LOGI(TAG, "[%s] LED value set to 0x%04X", __func__, value);
        }
        else
        {
            OB_LOGE(TAG, "[%s] Err:LED driver set_value func is null", __func__);
        }
    }
}

/**
 * @brief 获取LED当前值
 * @retval LED值（0~255），禁用时返回0
 */
uint8_t ledTaskGetValue(void)
{
    if (ledTaskDriver.attribute.enable)
    {
        if (ledTaskDriver.io != NULL && ledTaskDriver.io->get_value != NULL)
        {
            uint8_t value = ledTaskDriver.io->get_value();
            OB_LOGI(TAG, "[%s] LED value get: %d", __func__, value);
            return value;
        }
        else
        {
            OB_LOGE(TAG, "[%s] Err:LED driver get_value func is null", __func__);
        }
    }
    return 0;
}

/**
 * @brief 检查LED状态
 * @param led_value: 待检查的LED值
 * @retval true-匹配成功，false-匹配失败/禁用
 */
bool ledTaskCheckLed(uint8_t led_value)
{
    if (ledTaskDriver.attribute.enable)
    {
        if (ledTaskDriver.io != NULL && ledTaskDriver.io->check_led != NULL)
        {
            bool ret = ledTaskDriver.io->check_led(led_value, NULL);
            OB_LOGI(TAG, "[%s] LED check: led_value=%d, result=%d",__func__, led_value, ret);
            return ret;
        }
        else
        {
            OB_LOGE(TAG, "[%s] Err:LED driver check_led func is null", __func__);
        }
    }
    return false;
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

