/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_nfc.c
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-05
 */
#include "bsp_nfc.h"

/***************Variable***************/
const bsp_nfc_drive_io_t bsp_nfc = {
    .init = nfc_init,
    .loop = nfc_loop,
    .set_mode = nfc_set_mode,
    .is_wake = is_nfc_wake,
    .set_attr = nfc_set_attr,

};

// ------------------------------------------

const bsp_nfc_drive_io_t* bsp_nfc_get_driver(bsp_nfc_callback_t callback) {
    nfc_event_register_callback(callback);
    return &bsp_nfc;
}

