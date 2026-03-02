/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_key.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-01
 */

#include "bsp_key.h"

/*************************Variable*************************/
const bsp_key_drive_io_t bsp_key = {
    .init = module_keyInit,
    .scan = module_keyScanLoop,
    .sleep = module_keySleep,
    .check_wake = module_keyWake,
    .busy = module_isBusyKey,
    .handle = module_keySwitch,
    .pause = module_key_pause,
    .test = module_keyTestMode,
};

//--------------------------------------------------
const bsp_key_drive_io_t* bsp_key_get_driver(bsp_key_callback_t callback)
{
    keyEventRegister_callback(callback);
    return &bsp_key;
}

