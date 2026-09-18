/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_nfc.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-05
 */

#ifndef BSP_NFC__HH
#define BSP_NFC__HH

#include "config.h"
#include "NFC_API.h"

typedef nfc_event_callback_t bsp_nfc_callback_t;

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct{
    uint8_t (*init)(const nfc_attribute_t*);
    uint8_t (*set_attr)(const nfc_attribute_t*);
    void (*loop)(void);
    uint8_t (*set_mode)(uint8_t);
    uint8_t (*is_wake)(void);
    void (*scan_get_tick)(uint32_t);

}bsp_nfc_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_nfc_drive_io_t* bsp_nfc_get_driver(bsp_nfc_callback_t callback);

/**************************************/

#endif // BSP_NFC_HH

