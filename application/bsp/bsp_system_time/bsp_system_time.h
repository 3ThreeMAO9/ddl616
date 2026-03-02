/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: bsp_system_time.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-15
 *  */

#ifndef BSP_SYSTEM_TIME__HH
#define BSP_SYSTEM_TIME__HH

#include "config.h"
#include "system_timer.h"

typedef system_time_callback_t bsp_system_time_callback_t;
/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    void (*init)(uint8_t);
    uint16_t (*loop)(void);
    void (*time_out)(uint8_t, uint8_t, uint32_t);
    void (*sleep)(uint8_t);

    uint8_t (*timer_time_out)(const uint32_t);
    uint32_t (*timer_inc_cnt)(const uint32_t);
    uint32_t (*ms_tick)(void);

}bsp_system_time_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_system_time_drive_io_t* bsp_system_time_get_driver(bsp_system_time_callback_t callback);

/**************************************/

#endif 
