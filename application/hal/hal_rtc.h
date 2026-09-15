/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: hal_rtc.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */
#ifndef HAL_RTC_HH
#define HAL_RTC_HH

#include "config.h"
#include "time.h"
#include "rtc.h"

typedef void (*rtc_callback_t)(void);

/***********Macro***********/
/***********Enum***********/

/***********Struct***********/
typedef struct{
    uint32_t interrupt;
    uint32_t wake_flag;
    uint32_t time_out;
    uint32_t ble_time_out;
    uint32_t last_ticks;        // 上一次的10ms的计数
}rtc_handlet_t;

typedef struct{
    time_t utc_timestamp;       // 格林时间戳
    time_t local_timestamp;     // 本地时间戳
    int16_t time_zone;          // 时区  东8区为+32；西8区为-32
}rtc_time_info_t;


/***********Variable***********/

/***********Function***********/
void hal_rtc_init(rtc_callback_t callback);
uint32_t hal_get_rtc_time(void);
uint32_t hal_get_rtc_interrupt(void);
void hal_set_rtc_interrupt(uint32_t data);

void hal_set_rtc_local_timestamp(uint32_t timestamp);
time_t hal_get_rtc_local_timestamp(void);
void hal_set_rtc_utc_timestamp(uint32_t timestamp);
time_t hal_get_rtc_utc_timestamp(void);
void hal_set_rtc_time_zone(int16_t time_zone);
int16_t hal_get_rtc_time_zone(void);

uint32_t hal_get_rtc_utc_time(void);
time_t hal_get_rtc_to_utc_time(time_t timestamp);
time_t hal_set_default_time(void);

/*****************************/

#endif //HAL_RTC_HH

