/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_system_time.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-01-15
 *  */

#ifndef TASK_SYSTEM_TIME__HH
#define TASK_SYSTEM_TIME__HH

#include "config.h"
#include "bsp_system_time.h"


/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    uint8_t count;
    
}system_time_task_attribute_t;

typedef struct{
    const bsp_system_time_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     * 
     */
    system_time_task_attribute_t attribute;

}system_time_task_driver_t;

/***************Variable***************/


/***************Function***************/
void system_time_task_init(void);
uint16_t system_time_task_loop(void);
void system_time_task_sleep(uint8_t event);
void system_time_task_set_work_time(uint32_t downCount);
void system_time_task_set_function_time(uint32_t downCount);
void system_time_task_set_timer_timeout(uint8_t id, uint32_t downCount);
void system_time_task_set_wdt_timeout(uint8_t id, uint32_t downCount);

/**************************************/

#endif 
