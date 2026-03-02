/**
 * Copyright(C) 2024 GZ-OB. All Rights Reserved
 * File: bsp_flash.h
 * Desc: 
 * Ver : V1.00
 * Revision: Ren
 * Date: 2025-09-17
 *  */

#ifndef BSP_FLASH__HH
#define BSP_FLASH__HH

#include "config.h"
#include "flash.h"

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    void (*init)(flash_callback_t);

    void (*write_block)(uint32_t, uint16_t, uint8_t*, const uint16_t);
    void (*read_block)(uint32_t, uint16_t, uint8_t*, const uint16_t);

    void (*modify_block)(uint32_t, uint32_t, uint8_t, uint16_t, uint8_t*, uint16_t);
    void (*renew_area)(uint32_t, uint32_t, uint8_t, uint16_t, uint8_t*, uint16_t);

    void (*erase_write_safe)(uint32_t, uint32_t, uint8_t*, uint16_t);
    void (*erase_write)(uint32_t, uint8_t*, uint16_t);

    uint8_t (*erase_sectors)(uint32_t, uint16_t);
    uint8_t (*write_sectors)(uint32_t, uint8_t*, uint16_t);
    uint8_t (*read_sectors)(uint32_t, uint8_t*, uint16_t);

    void (*write_flag)(uint32_t);
    uint8_t (*invalid)(uint32_t);

}bsp_flash_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_flash_drive_io_t* bsp_flash_get_driver(void);

/**************************************/

#endif 
