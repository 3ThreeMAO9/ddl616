/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_led.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */
#ifndef TASK_LED_H
#define TASK_LED_H

#include "config.h"
#include "bsp_led.h"

typedef bsp_led_callback_t task_led_callback_t;
/***********Macro***********/

/***********Enum***********/

/***********Struct***********/
typedef struct{
    uint8_t enable;
}led_task_attribute_t;

typedef struct{
    const bsp_led_drive_io_t *io;
    led_task_attribute_t attribute;

}led_task_driver_t;

/***********Variable***********/

/***********Function***********/
void ledTaskInit(void);
void ledTaskHandle(uint8_t event_type, uint16_t event_code);
uint8_t ledTaskIsEnable(void);
void ledTaskSetEnable(uint8_t enable);
void ledTaskSleep(void);
void ledTaskWake(void);
void ledTaskLoop(void);
uint8_t ledTaskGetValue(void);
void ledtaskSetValue(uint16_t value);
bool ledTaskCheckLed(uint8_t led_value);

/*****************************/

#endif // TASK_LED_H

