/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: motor.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-1-23
 *  */

#ifndef MOTOR__HH
#define MOTOR__HH

#include "config.h"
#include "hal_pwm.h"
#include "hal_gpio.h"

/*****************Macro****************/
//user config
#define SET_MOTOR_LA_HIGH                           HAL_GPIO_Write(MOTOR_LA_GPIO,MOTOR_LA_PIN,1)
#define CLR_MOTOR_LA_LOW                            HAL_GPIO_Write(MOTOR_LA_GPIO,MOTOR_LA_PIN,0)

#define SET_MOTOR_LB_HIGH                           HAL_GPIO_Write(MOTOR_LB_GPIO,MOTOR_LB_PIN,1)
#define CLR_MOTOR_LB_LOW                            HAL_GPIO_Write(MOTOR_LB_GPIO,MOTOR_LB_PIN,0)

#define MOTOR_LA_SLEEP()                                                                        \
    {                                                                                           \
        HAL_GPIO_Init(MOTOR_LA_GPIO,MOTOR_LA_PIN,HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);  \
        CLR_MOTOR_LA_LOW;                                                                       \
    }

#define MOTOR_LB_SLEEP()                                                                        \
    {                                                                                           \
        HAL_GPIO_Init(MOTOR_LB_GPIO,MOTOR_LB_PIN,HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE);  \
        CLR_MOTOR_LB_LOW;                                                                       \
    }

/*****************Enum*****************/

typedef enum{
    MOTOR_HANDLE_NULL = 0,
    MOTOR_HANDLE_UNLOCK,
    MOTOR_HANDLE_PCBA_TEST,
    MOTOR_HANDLE_DEVICE_TEST,
    MOTOR_HANDLE_OPEN,
    MOTOR_HANDLE_CLOSE,
    MOTOR_HANDLE_CNT,
}motor_handle_e;

typedef enum{
    MOTOR_STEP_IDLE = 0,
    MOTOR_STEP_INIT,

    MOTOR_STEP_UNLOCK,
    MOTOR_STEP_UNLOCK_BREAK,
    MOTOR_STEP_UNLOCK_REVERSE,
    MOTOR_STEP_UNLOCK_REVERSE_BREAK,

    MOTOR_STEP_BRAKE,
    MOTOR_STEP_SLEEP,

    MOTOR_STEP_CNT,
}motor_step_e;

/****************Struct****************/


/***************Variable***************/


/***************Function***************/
void motorDriver(uint8_t motorStep, uint8_t direction);


/**************************************/

#endif 
