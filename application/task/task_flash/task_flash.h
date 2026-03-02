/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: task_flash.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-09-17
 *  */

#ifndef TASK_FLASH__HH
#define TASK_FLASH__HH

#include "config.h"
#include "bsp_flash.h"

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    uint8_t flag;
    
}flash_task_attribute_t;

typedef struct{
    const bsp_flash_drive_io_t *io;

    /**
     * @brief Attribute of this driver
     * 
     */
    flash_task_attribute_t attribute;

}flash_task_driver_t;


/***************Variable***************/


/***************Function***************/
void flash_task_init(void);

/**************************************/

#endif 
