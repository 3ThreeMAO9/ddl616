/*
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: module_battery.h
 * Desc:
 * Ver : V1.00
 * Revision: Ren
 * Date: 2024-11-06
 * */

#ifndef MODULE_BATTERY__HH
#define MODULE_BATTERY__HH

#include "config.h"

typedef void (*battery_callback_t)(uint8_t);

/********************Macro********************/
#define BATTERY_VOLTAGE_PIN_INIT()                                                                  \
    do                                                                                              \
    {                                                                                               \
        HAL_GPIO_Init(BATTERY_ADC_GPIO, BATTERY_ADC_PIN, HAL_GPIO_MODE_ANALOG, HAL_GPIO_PULL_NONE); \
    } while (0)

#define BATTERY_VOL_SAMPLE_CNT                (16)

#define BATTERY_VOL_COMPENSATION_VALUE        (250)   // uint: mV        电压补偿值(二极管压降)

#define BATTERY_VOL_RAISE_UPDATE_VALUE        (1000)  // uint: mV

#define BATTERY_VOL_LOW_WARN_TH               (4800)  // uint: mV        (=20%)
#define BATTERY_VOL_LOW_SYSTEM_LOCK_TH        (4200)  // uint: mV        (=0%)

#define BATTERY_VOL_PERCENT_0                 (4500)  // uint: mV
#define BATTERY_VOL_PERCENT_100               (6000)  // uint: mV

#define BATTERY_LOW_WARN_TIME                 (501)   // unit: ms

/********************Enum********************/
typedef enum{
    BATTERY_STATE_NORMAL = 0,
    BATTERY_STATE_LOW_WARN,
    BATTERY_STATE_LOW_SYSTEM_LOCK,

}battery_state_e;

typedef enum{
    BATTERY_EVENT_STATE_CHANGE = 0,
    BATTERY_EVENT_STATE_LOW_WARN,

}battery_event_e;

/********************Struct********************/
typedef struct{
    uint16_t voltage;       // unit: ms
    uint8_t percent;        // 0~100
    uint8_t lowState;
    uint32_t timeOut;

}battery_handle_t;

/********************Variable********************/



/********************Function********************/
void module_batteryInit(void);
uint8_t module_batteryReadState(void);
uint8_t module_batteryReadPercent(void);
uint16_t module_batteryReadVoltage(void);
void module_batteryScan(uint16_t compensationValue);
void module_batteryLoop(void);
void batteryEventRegister_callback(battery_callback_t callback);
uint16_t TestBatteryVoltage(void);

/***********************************************/

#endif
