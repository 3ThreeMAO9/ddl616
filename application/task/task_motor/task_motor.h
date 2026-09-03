/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_motor.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2024-12-17
 *  */

#ifndef TASK_MOTOR__HH
#define TASK_MOTOR__HH

#include "config.h"
#include "bsp_motor.h"


typedef bsp_motor_callback_t task_motor_callback_t;
/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    uint8_t enable;
    
}motor_task_attribute_t;

typedef struct{
    const bsp_motor_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     * 
     */
    motor_task_attribute_t attribute;

}motor_task_driver_t;

/***************Variable***************/


/***************Function***************/
void motorTaskInit(void);
void motorTaskLoop(void);
uint8_t motorTaskReadMotorBusy(void);
uint8_t motorTaskReadDoorState(void);
uint8_t motorTaskWriteDoorState(void);
uint16_t motorTaskReadWakeFlag(void);
void motorTaskHandle(uint8_t handleCmd, uint8_t value);
void motorTaskAutoLockHandle(uint8_t flag, uint8_t changeflag);
void motorTaskSetVacationModeWarnTime(uint32_t time);
void motorTaskSetMotorWarnTime(uint32_t time);

/**************************************/

#endif 
