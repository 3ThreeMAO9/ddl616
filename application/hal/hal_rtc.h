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
#include "rtc.h"

typedef void (*rtc_callback_t)(void);

/***********Macro***********/
/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/
void hal_rtc_init(rtc_callback_t callback);
/*****************************/

#endif //HAL_RTC_HH

