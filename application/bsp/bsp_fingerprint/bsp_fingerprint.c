
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_fingerprint.c
 * Desc: 指纹/ 指静脉模块板级抽象层
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-10
 */
#include "bsp_fingerprint.h"

/***************Variable***************/
static const bsp_fp_drive_io_t bsp_fp = {
    .init = fp_init_API,
    .loop = fp_loop_API,
    .set_mode = fp_set_mode_API,
    .set_attr = fp_set_attr_API,
    .is_wake = fp_is_wake_API,
    .is_busy = fp_is_busy_API,
    
#if (FP_ENABLE_LED_CONTROL)
    .control_led = fp_control_led_API,
#endif

};

// ------------------------------------------
const bsp_fp_drive_io_t* bsp_fp_get_driver(void) {
    return &bsp_fp;
}

