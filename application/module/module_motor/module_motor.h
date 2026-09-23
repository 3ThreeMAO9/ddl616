/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: module_motor.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2024-11-20
 *  */

#ifndef MODULE_MOTOR__HH
#define MODULE_MOTOR__HH

#include "config.h"
#include "motor.h"

typedef void (*motor_callback_t)(uint8_t, uint32_t);

/*****************Macro****************/
//user config...
#define MOTOR_UNLOCK_TIME_OUT                       (250)       // unit: ms
#define MOTOR_UNLOCK_BRAKE_TIME_OUT                 (5 * 1000)  // unit: ms
#define MOTOR_UNLOCK_REVERSE_TIME_OUT               (250)       // unit: ms
#define MOTOR_BRAKE_TIME_OUT                        (10)        // unit: ms


/*****************Enum*****************/
typedef enum{
    MOTOR_EVENT_UNLOCKED = 0,
    MOTOR_EVENT_LOCKED,
    MOTOR_EVENT_LOCKED_ROTOR,
    MOTOR_EVENT_TIME_OUT,
    MOTOR_EVENT_VACATION_MODE_WARN,
    
    MOTOR_EVENT_IDLE,

    MOTOR_EVENT_DEVICE_TEST,

    MOTOR_EVENT_OPENED,
    MOTOR_EVENT_CLOSED,
}motor_event_e;

typedef enum{
    MOTOR_TYPE_NULL = 0,
    MOTOR_TYPE_KEY = 1,     //机械钥匙
    MOTOR_TYPE_APP = 2,     //APP钥匙
    MOTOR_TYPE_AUTO_LOCK = 3,   //自动上锁
    MOTOR_TYPE_USER = 4,    //用户验证
}motor_type_e;




/****************Struct****************/
typedef struct{
    uint8_t doorState;
    uint8_t doorStateLast;
    uint8_t doorStateChange;
    
    uint8_t handle;
    uint8_t busy;
    uint8_t direction;
    uint8_t motorStep;
    uint8_t duty;
    
    uint8_t protectFlag;

    uint8_t warnBusy;
    uint8_t warnCnt;
    uint8_t motorwarnCnt;
    uint32_t warnTimeOut;
    uint32_t motorwarnTimeOut;
    uint32_t scanTimeOut;
    uint32_t timeOut;
    uint32_t warnSetFlag;

    uint8_t deviceTest;


}motor_handle_t;

/***************Variable***************/


/***************Function***************/
void module_motorLoop(void);
void module_motorHandle(uint8_t handle, uint16_t value);
void module_motorInit(void);
uint8_t module_motorReadDoorState(void);
uint8_t module_motorReadMotorBusyAndWarn(void);
uint8_t module_motorReadMotorBusy(void);
uint16_t module_motorWake(void);
void motorEventRegister_callback(motor_callback_t callback);
void module_motorVacationWarnTime(uint32_t warn_time);
void module_motorWarnTime(uint32_t warn_time);
void set_agingmode(uint8_t mode);
uint8_t isagingmode(void);
/**************************************/

#endif 
