/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: task_key.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */
#ifndef TASK_KEY_H
#define TASK_KEY_H

#include "config.h"
#include "bsp_key.h"

typedef bsp_key_callback_t task_key_callback_t;
/***********Macro***********/

/***********Enum***********/

/***********Struct***********/
typedef struct{
    uint8_t enable[KEY_TYPE_CNT];
}key_task_attribute_t;

typedef struct{
    const bsp_key_drive_io_t *io;
    key_task_attribute_t attribute;

}key_task_driver_t;

/***********Variable***********/

/***********Function***********/
void keyTaskInit(uint8_t type);
void keyTaskPowerOnInit(void);
void keyTaskLoop(void);
uint8_t key_task_check_wake(void);
void key_task_wake_config(uint8_t keyboard_wake);
uint8_t key_task_sleep(uint8_t type);
void keyTaskHandle(uint8_t keyType, uint8_t enable);
uint8_t keyTaskIsBusy(uint8_t type);
void keyboard_task_pause(uint32_t value);
void keyboard_test_mode(uint8_t mode);
void keyTaksInit(uint8_t type);
void keyTaksParameter(const uint8_t* sensitivity);
void key_task_wake(void);
/*****************************/

#endif // TASK_KEY_H

