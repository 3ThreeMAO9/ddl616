/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_key.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */
#include "task_key.h"
#include "task_face.h"
#include "task_radar.h"
#include "task_led.h"
#include "task_fingerprint.h"
#include "task_system_time.h"
#include "task_sleep.h"
#include "event.h"
#include "msg_protocol.h"
#include "user_parameter.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "task_key"

static key_task_driver_t g_key_task_driver;

#include <stdint.h>

// 功能：检测是否连续输入 1 -> 5 -> 9 -> #
// 输入：num  0~9 的数字
// 返回：1 = 连续输入；0 = 不满足
uint8_t check_rest_7258_test(uint8_t num)
{
    // 用静态变量保存最近4次的输入
    static uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    
    // 旧数据前移，新数据放最后
    buf[0] = buf[1];
    buf[1] = buf[2];
    buf[2] = buf[3];
    buf[3] = num;

    // 判断是否连续 1 -> 5 -> 9 -> #
    if (buf[0] == 1 && buf[1] == 5 && buf[2] == 9 && buf[3] == 11)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void key_task_callback(uint8_t keyType,uint8_t key_value)
{
    // OB_LOGD(TAG, "[%s]  keyType=%d,key_value=%d",__func__, keyType,key_value);
    switch (keyType)
    {
    case KEY_TYPE_TAMPER_KEY:
        if(true == g_key_task_driver.attribute.enable[KEY_TYPE_TAMPER_KEY])
        {
            OB_LOGD(TAG, "KEY_TYPE_TAMPER_KEY event=%d", key_value);
            if(key_value == KEY_EVENT_TAMPER_PRESS){
                if (get_user_parameter(PARAMETER_TAMPER_ALARM) == Enabled){
                    uart_msg_tamper_key_warn(EVENT_CODE_TAMPER_WARN);
                }
                // baseEventPush(Q_HANDLE_SIG, HANDLE_EVENT_TAMPER_WARN);
            }
        }
        break;
    case KEY_TYPE_KEY_BOARD:
        if(true == g_key_task_driver.attribute.enable[KEY_TYPE_KEY_BOARD])
        {
            OB_LOGD(TAG, "key_value = %d", key_value);
            // if(key_value == 1)
            //     face_task_set_mode(FACE_MODE_VERIFY_DEMO);
            // else if(key_value == 2)
            //     face_task_set_mode(FACE_MODE_VERIFY);
            // else if(key_value == 3)
            //     face_task_set_mode(FACE_MODE_REGISTER);
            // else if(key_value == 4)
            //     face_task_set_mode(FACE_MODE_VERIFY_DELETE);
            // else if(key_value == 5)
            // {
            //     uint16_t id = 0x09;
            //     face_delete_params_t del_id;

            //     del_id.page_id = BIG_LITTLE_SWAP16(id);
            //     face_task_delete_face(del_id);
            // }
            radar_task_handle(RADAR_HANDLE_STAY, Disabled);     // 按键事件产生，关闭逗留检测
            uart_msg_key_board(EVENT_CODE_KEY_PRESS, key_value);
            if(ledTaskCheckLed(key_value))    //对应的灯是否使能
                ledTaskHandle(LED_EVENT_KEY_BOARD, key_value);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            baseEventPush(Q_KEY_BOARD_PRESS_SIG, key_value);

            if(check_rest_7258_test(key_value))
            {
                reset7258_handle();
            }
        }
        break;
    case KEY_TYPE_LONG_RELEASE:
        OB_LOGD(TAG, "KEY_TYPE_LONG_RELEASE event=%d", key_value);
        break;
    default:
        break;
    }
}

void key_task_wake(void)
{
    uart_msg_key_board(EVENT_CODE_KEY_PRESS, 1);
}

uint8_t key_task_sleep(uint8_t type)
{
    return g_key_task_driver.io->sleep(type);
}

uint8_t key_task_check_wake(void)
{
    return g_key_task_driver.io->check_wake();
}

void key_task_wake_config(uint8_t keyboard_wake)
{
    g_key_task_driver.io->wake_config(keyboard_wake);
}

uint8_t keyTaskBusy(uint8_t type)
{
    return g_key_task_driver.io->busy(type);
}

void keyTaskHandle(uint8_t keyType,uint8_t enable)
{
    g_key_task_driver.attribute.enable[keyType] = enable;
    g_key_task_driver.io->handle(keyType, enable);
}

void keyTaksInit(uint8_t type)
{
    g_key_task_driver.io->init(type);
}

void keyTaksParameter(const uint8_t* sensitivity)
{
    g_key_task_driver.io->parameter((void*)sensitivity);
}

void keyTaskPowerOnInit(void)
{
    OB_LOGD(TAG, "[%s]", __func__);
    g_key_task_driver.io = bsp_key_get_driver(key_task_callback);
    if (NULL != g_key_task_driver.io)
    {
        keyTaksInit(KEY_TYPE_KEY_BOARD);
        keyTaskHandle(KEY_TYPE_KEY_BOARD, true);
        keyTaksInit(KEY_TYPE_TAMPER_KEY);
        keyTaskHandle(KEY_TYPE_TAMPER_KEY, true);
    }
    else
    {
        OB_LOGE(TAG, "Err: key driver is null");
    }
}

void keyboard_task_pause(uint32_t value)
{
    g_key_task_driver.io->pause(value);
}

void keyTaskLoop(void)
{
    g_key_task_driver.io->scan();
}

void keyBoardTaskTestMode(uint8_t mode)
{
    g_key_task_driver.io->test(mode);
}

/*****************************/


