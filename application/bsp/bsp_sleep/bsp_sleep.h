/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: bsp_sleep.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-15
 *  */

#ifndef BSP_SLEEP__HH
#define BSP_SLEEP__HH

#include "config.h"
#include "sleep.h"

typedef sleep_event_callback_t bsp_sleep_config_callback_t;
/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    void (*init)(void);
    void (*loop)(void);

}bsp_sleep_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_sleep_drive_io_t* bsp_sleep_get_driver(bsp_sleep_config_callback_t callback);

/**************************************/

#endif 
