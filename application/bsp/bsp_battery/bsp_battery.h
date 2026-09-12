/*
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: bsp_battery.h
 * Desc:
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-03-14
 * */

#ifndef BSP_BATTERY__HH
#define BSP_BATTERY__HH

#include "config.h"
#include "module_battery.h"

typedef battery_callback_t bsp_battery_callback_t;

/********************Macro********************/


/********************Enum********************/


/********************Struct********************/
typedef struct{
    void (*init)(void);
    void (*loop)(void);
    void (*scan)(uint16_t);
    uint16_t (*voltage)(void);
    uint8_t (*percent)(void);
    uint8_t (*state)(void);
}bsp_battery_drive_io_t;

/********************Variable********************/



/********************Function********************/
const bsp_battery_drive_io_t* bsp_battery_get_driver(bsp_battery_callback_t callback);

/***********************************************/

#endif
