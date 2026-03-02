/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: hal_wdt.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */
#ifndef HAL_WDT_HH
#define HAL_WDT_HH

#include "config.h"
#include "wdt.h"

typedef void (*wdt_callback_t)(void);

/***********Macro***********/
#define FEED_DOG()            WDT_ReloadCounter()
#define WDT_CLOSE()           WDT_Close()
#define WDT_IRQ_TIME_OUT      (120)        //unit: ms

/***********Enum***********/

/***********Struct***********/

/***********Variable***********/

/***********Function***********/
void clear_feed_dog_cnt(void);
void hal_wdt_init(wdt_callback_t callback);
void hal_wdt_feed_dog(void);
/*****************************/

#endif //HAL_WDT_HH

