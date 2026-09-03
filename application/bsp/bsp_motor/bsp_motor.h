/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: bsp_motor.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2024-12-17
 *  */

#ifndef BSP_MOTOR__HH
#define BSP_MOTOR__HH

#include "config.h"
#include "module_motor.h"

typedef motor_callback_t bsp_motor_callback_t;

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    void (*init)(void);
    void (*loop)(void);
    void (*handle)(uint8_t, uint16_t);
    uint8_t (*motorBusy)(void);

}bsp_motor_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_motor_drive_io_t* bsp_motor_get_driver(bsp_motor_callback_t callback);

/**************************************/

#endif 
