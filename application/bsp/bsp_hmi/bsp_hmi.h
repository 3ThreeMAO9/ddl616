/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: bsp_hmi.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-15
 *  */

#ifndef BSP_HMI__HH
#define BSP_HMI__HH

#include "config.h"
#include "module_hmi.h"

typedef hmi_callback_t bsp_hmi_callback_t;
/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    void (*init)(void);
    void (*loop)(void);
    uint32_t (*setState)(uint8_t, uint8_t);
    void (*config)(uint8_t);
    void (*tamperwarn)(uint32_t);

}bsp_hmi_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_hmi_drive_io_t* bsp_hmi_get_driver(bsp_hmi_callback_t callback);

/**************************************/

#endif 
