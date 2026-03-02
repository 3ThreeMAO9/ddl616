
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_fingerprint.h
 * Desc: 指纹/ 指静脉模块板级抽象层
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-10
 */
#ifndef __BSP_FINGERPRINT_H
#define __BSP_FINGERPRINT_H

#include "fingerprint_api.h"

typedef fingerprint_callback_t bsp_fp_callback_t;

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct {
    void (*init)(bsp_fp_callback_t);
    uint8_t (*loop)(void);
    uint8_t (*set_mode)(uint8_t, bsp_fp_callback_t, void*, uint8_t);
    uint8_t (*set_attr)(void*, uint8_t);
    uint8_t (*is_wake)(void);
    uint8_t (*is_busy)(void);
#if (FP_ENABLE_LED_CONTROL)
    uint8_t (*control_led)(uint8_t);
#endif
} bsp_fp_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_fp_drive_io_t* bsp_fp_get_driver(void);

/**************************************/

#endif /* __BSP_FINGERPRINT_H */
