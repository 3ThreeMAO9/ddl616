/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: led.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-03
 */

#include "led.h"
#include "system_timer.h"
#include <string.h>

#define OB_LOG_LEVEL OB_LOG_LEVEL_NONE
#include "ob_log.h"
#define TAG "led"

static led_event_callback_t led_callback;

static void ledEvent_callback(uint8_t ledType, uint8_t ledEvent, uint8_t ledState, uint8_t ledValue)
{
    if (NULL != led_callback)
    {
        led_callback(ledType, ledEvent, ledState, ledValue);
    }
    else
    {
        OB_LOGE(TAG, "Err:led callback is null");
    }
}

void ledEventRegister_callback(led_event_callback_t callback)
{
    led_callback = callback;
    OB_LOGD(TAG,"[%s] led_register_callback: %X", __func__, callback);
}

void led_init(void)
{
    LED_POWER_INIT(1);
}

void led_sleep(void)
{
    SET_LED_POWER(0);
}

void led_hanlde(uint8_t event_type, uint16_t event_code)
{
    switch (event_type)
    {
    case LED_EVENT_ALL_ON:
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_ALL_ON, NULL, 0);
        break;
    case LED_EVENT_ALL_OFF:
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_ALL_OFF, NULL, 0);
        break;
    case LED_EVENT_SET_VALUE:
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_SET_VALUE, NULL, 0);
        break;
    case LED_EVENT_KEY_BOARD:
        break;
    case LED_EVENT_WAKE_UP:
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_WAKE_UP, NULL, NULL);
        break;
    case LED_EVENT_UNLOCK:
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_UNLOCK, NULL, NULL);
        break;
    case LED_EVENT_LOCK:
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_LOCK, NULL, NULL);
        break;
    case LED_EVENT_FAILED:
        ledEvent_callback(LED_TYPE_KEY_BOARD, LED_EVENT_FAILED, NULL, 0);
        break;
    default:
        break;
    }
}


void led_loop(void)
{

}

