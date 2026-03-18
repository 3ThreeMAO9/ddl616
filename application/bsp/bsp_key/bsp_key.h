/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_key.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#ifndef BSP_KEY__HH
#define BSP_KEY__HH

#include "config.h"
#include "key.h"

typedef key_event_callback_t bsp_key_callback_t;

/*************************Macro*************************/

/*************************Enum*************************/

/*************************Struct*************************/
typedef struct {
    uint8_t (*init)(uint8_t);
    void (*scan)(void);
    uint8_t (*sleep)(uint8_t);  
    uint8_t (*check_wake)(void);
    void (*wake_config)(uint8_t);
    uint8_t (*busy)(uint8_t);
    void (*handle)(uint8_t, uint8_t);
    void (*pause)(uint32_t);
    void (*test)(uint8_t);
} bsp_key_drive_io_t;

/*************************Variable*************************/

/*************************Function*************************/
const bsp_key_drive_io_t* bsp_key_get_driver(bsp_key_callback_t callback);

/**********************************************************/

#endif

