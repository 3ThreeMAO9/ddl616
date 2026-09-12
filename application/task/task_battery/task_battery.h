/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_battery.h
 * Desc:
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-03-14
 * */

#ifndef TASK_BATTERY__HH
#define TASK_BATTERY__HH

#include "config.h"
#include "bsp_battery.h"

typedef bsp_battery_callback_t task_battery_callback_t;

/********************Macro********************/

/********************Enum********************/

/********************Struct********************/
typedef struct{
    uint8_t enable;

}battery_task_attribute_t;

typedef struct{
    const bsp_battery_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     *
     */
    battery_task_attribute_t attribute;

}battery_task_driver_t;

/********************Variable********************/

/********************Function********************/
void batteryTaskInit(void);
void batteryTaskLoop(void);
uint8_t batteryTaskReadState(void);
uint16_t batteryTaskReadVoltage(void);
uint8_t batteryTaskReadPercent(void);
void batteryTaskScan(uint16_t compensationValue);
void batteryTaskWarnConfig(uint8_t flag);
bool IsBatteryLow(void);

/***********************************************/

#endif
