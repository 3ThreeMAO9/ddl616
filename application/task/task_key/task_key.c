/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_key.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */
#include "task_key.h"



#include "task_fingerprint.h"
#include "task_system_time.h"
#include "task_sleep.h"
#include "task_player.h"
#include "event.h"
#include "msg_protocol.h"
#include "user.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "task_key"

static key_task_driver_t g_key_task_driver;

#include <stdint.h>

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
                baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_BREAK_LONG_PRESS);
            }
        }
        break;
    case KEY_TYPE_KEY_BOARD:
        if(true == g_key_task_driver.attribute.enable[KEY_TYPE_KEY_BOARD])
        {
            OB_LOGD(TAG, "key_value = %d", key_value);
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            baseEventPush(Q_KEY_BOARD_PRESS_SIG, key_value);
        }
        break;
    case KEY_TYPE_LONG_RELEASE:
        OB_LOGD(TAG, "KEY_TYPE_LONG_RELEASE event=%d", key_value);
        break;
    case KEY_TYPE_VOICE_MODE:
        if (key_value == KEY_NUM_0)
        {
            OB_LOGD(TAG, "KEY_TYPE_VOICE_MODE");
            system_time_task_set_work_time(WORK_TIME_OUT_VAULE);
            baseEventPush(Q_HANDLE_SIG, EVENT_RESULT_VOICE_MODE);
        }
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

void keyTaskInit(uint8_t type)
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
        keyTaskInit(KEY_TYPE_KEY_BOARD);
        keyTaskHandle(KEY_TYPE_KEY_BOARD, true);
        keyTaskInit(KEY_TYPE_TAMPER_KEY);
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


