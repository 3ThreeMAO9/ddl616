/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_hmi.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-15
 *  */

#ifndef TASK_HMI__HH
#define TASK_HMI__HH

#include "config.h"
#include "bsp_hmi.h"


/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/

typedef struct{
    uint8_t silentFlag;
    
}hmi_task_attribute_t;

typedef struct{
    const bsp_hmi_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     * 
     */
    hmi_task_attribute_t attribute;

}hmi_task_driver_t;

/***************Variable***************/


/***************Function***************/
void hmiTaskInit(void);
void hmiTaskLoop(void);
void hmiTaskConfig(uint8_t sleepFlag);
uint32_t hmiTaskSetState(uint8_t state);
void hmiTaskSetAllowSelintFlag(uint8_t allowSelintFlag);
uint8_t isallowSlient(void);
void hmi_task_tamper_warn_time(uint32_t time);
uint8_t hmi_task_tamper_warn_is_busy(void);

/**************************************/

#endif 
