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

#include "task_nfc.h"

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

    if (fp_task_is_busy()) {
        OB_LOGD(TAG, "fp_task_is_busy");
        baseEventPush(Q_HANDLE_SIG, HANDLE_EVENT_SLEEP_BUSY);
        return false;
    }

    if (face_task_is_busy()) {
        OB_LOGD(TAG, "face_task_is_busy");
        baseEventPush(Q_HANDLE_SIG, HANDLE_EVENT_SLEEP_BUSY);
        return false;
    }

    if (!key_task_sleep(KEY_TYPE_KEY_BOARD)) {
        OB_LOGD(TAG, "key_task_is_busy");
        baseEventPush(Q_HANDLE_SIG, HANDLE_EVENT_SLEEP_BUSY);
        return false;
    }
    ledTaskSleep();
    uartTaskSleep();

    OB_LOGD(TAG, "Enter Sleep");
    system_time_task_sleep(ENTER_SLEEP);
    sleep_task_driver.attribute.wake_source = WAKE_SOURCE_NULL;

    return true;
}

static uint8_t exit_sleep_event_scan(void){
    sleep_task_driver.attribute.wake_source |= key_task_check_wake();   //  触摸
    sleep_task_driver.attribute.wake_source |= fp_task_is_wake();       //  指纹
    sleep_task_driver.attribute.wake_source |= nfc_task_is_wake();      //  NFC
    sleep_task_driver.attribute.wake_source |= system_timer_loop();     //  WDT定时
    sleep_task_driver.attribute.wake_source |= uart_task_is_wake();     //  串口

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

    if(WAKE_SOURCE_KEY_BOARD & sleep_task_driver.attribute.wake_source)
    {
        key_task_wake_config(1);
    }
    else
    {
        key_task_wake_config(0);
    }

#if (Enabled == WAKE_STAT_ENABLE)
    Wake_allCnt++;
#endif
    ob_log_init();
    system_time_task_sleep(EXIT_SLEEP);
    uartTaskWake();
    ledTaskWake();

    uint8_t wake_type = WAKE_UP_TYPE_NULL;
    if (sleep_task_driver.attribute.wake_source & WAKE_SOURCE_RADAR)
        wake_type = WAKE_UP_TYPE_RADAR;
    else if (sleep_task_driver.attribute.wake_source & WAKE_SOURCE_KEY_BOARD)
        wake_type = WAKE_UP_TYPE_KEY_BOARD;
    handleEventPush(HANDLE_EVENT_WAKE, wake_type);
#if (Enabled == WAKE_STAT_ENABLE)
    OB_LOGD(TAG, "all[%u] keyBoard[%u] setKey[%u] switchKey[%u] NFC[%u] doorState[%u] hotWarn[%u] remote[%u]", \
            Wake_allCnt, Wake_keyBoardCnt, Wake_setKeyCnt, \
            Wake_switchKeyCnt, Wake_NFCCnt, Wake_doorStateCnt, \
            Wake_hotWarnCnt, Wake_remoteCnt);
#endif
    OB_LOGD(TAG, "Sleep[%s][%x]", "Exit", sleep_task_driver.attribute.wake_source);

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
