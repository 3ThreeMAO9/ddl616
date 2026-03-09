/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_sleep.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-22
 */

#include "task_sleep.h"

#include "task_key.h"
#include "task_system_time.h"
#include "task_led.h"
#include "task_face.h"
#include "task_uart.h"
#include "task_fingerprint.h"

#include "event.h"

#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "task_sleep"

/***************Variable***************/
#if (Enabled == WAKE_STAT_ENABLE)

u8 Wake_allCnt = 0;
u8 Wake_keyBoardCnt = 0;
u8 Wake_setKeyCnt = 0;
u8 Wake_switchKeyCnt = 0;
u8 Wake_NFCCnt = 0;
u8 Wake_doorStateCnt = 0;
u8 Wake_hotWarnCnt = 0;
u8 Wake_remoteCnt = 0;

#endif

static sleep_task_driver_t sleep_task_driver;

// ------------------------------------------

static uint8_t enter_sleep_event_deal(void){
    OB_LOGD(TAG, "event[%s]", "Enter");

    // if (!motor_task_sleep(ENTER_SLEEP)){
    //     return false;
    // }
    if (!key_task_sleep(KEY_TYPE_KEY_BOARD)) {
        return false;
    }
    ledTaskSleep();
    uartTaskSleep();
    HAL_GPIO_Write(VOICE_EN_GPIO, VOICE_EN_PIN, 0); // 前板喇叭功放

    OB_LOGD(TAG, "Enter Sleep");
    system_time_task_sleep(ENTER_SLEEP);
    sleep_task_driver.attribute.wake_source = WAKE_SOURCE_NULL;

    return true;
}

static uint8_t exit_sleep_event_scan(void){
    sleep_task_driver.attribute.wake_source |= key_task_check_wake();
    sleep_task_driver.attribute.wake_source |= fp_task_is_wake();
    // sleep_task_driver.attribute.wake_source |= nfc_task_is_wake();
    sleep_task_driver.attribute.wake_source |= system_timer_loop();
#if (Enabled == WAKE_STAT_ENABLE)
    switch (sleep_task_driver.attribute.wake_source)    {
        case WAKE_SOURCE_KEY_BOARD:
            Wake_keyBoardCnt++;
            break;
        case WAKE_SOURCE_SET_KEY:
            Wake_setKeyCnt++;
            break;
        case WAKE_SOURCE_SWITCH_KEY:
            Wake_switchKeyCnt++;
            break;
        case WAKE_SOURCE_NFC:
            Wake_NFCCnt++;
            break;
        case WAKE_SOURCE_DOOR_SENSOR:
            Wake_doorStateCnt++;
            break;
        case WAKE_SOURCE_HOT_WARN:
            Wake_hotWarnCnt++;
            break;
        case WAKE_SOURCE_REMOTE:
            Wake_remoteCnt++;
            break;
        default:
            break;
    }
#endif
    //check wake source
    if (WAKE_SOURCE_NULL == sleep_task_driver.attribute.wake_source){
        return false;
    }
#if (Enabled == WAKE_STAT_ENABLE)
    Wake_allCnt++;
#endif
    ob_log_init();
    system_time_task_sleep(EXIT_SLEEP);
    uartTaskWake();
    ledTaskWake();
    HAL_GPIO_Write(VOICE_EN_GPIO, VOICE_EN_PIN, 1); // 前板喇叭功放

    baseEventPush(Q_HANDLE_SIG, HANDLE_EVENT_WAKE);
#if (Enabled == WAKE_STAT_ENABLE)
    OB_LOGD(TAG, "all[%u] keyBoard[%u] setKey[%u] switchKey[%u] NFC[%u] doorState[%u] hotWarn[%u] remote[%u]", \
            Wake_allCnt, Wake_keyBoardCnt, Wake_setKeyCnt, \
            Wake_switchKeyCnt, Wake_NFCCnt, Wake_doorStateCnt, \
            Wake_hotWarnCnt, Wake_remoteCnt);
#endif
    OB_LOGD(TAG, "Sleep[%s]", "Exit");

    return true;
}

static uint8_t sleep_task_event_callback(uint8_t event){
    if (ENTER_SLEEP == event){
        return enter_sleep_event_deal();
    }
    else{
        return exit_sleep_event_scan();
    }
}

void sleep_task_set_flag(void)
{
    // 判忙，如busy则不设置标志
    OB_LOGD(TAG, "Sleep[%s]", "Flag");
    sleep_task_driver.attribute.flag = true;
}

void sleep_task_init(void){
    OB_LOGD(TAG, "Init");

    sleep_task_driver.io = bsp_sleep_get_driver(sleep_task_event_callback);

    if (NULL != sleep_task_driver.io){
        sleep_task_driver.io->init();
        sleep_task_driver.attribute.flag = false;
    }
}

void sleep_task_loop(void){
    if (NULL == sleep_task_driver.io){
        return;
    }

    if (sleep_task_driver.attribute.flag){
        sleep_task_driver.attribute.flag = false;
        sleep_task_driver.io->loop();
    }
}
