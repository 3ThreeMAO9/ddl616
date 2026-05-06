/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_face.h
 * Desc: 人脸识别模块板级抽象层
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 */
#ifndef __BSP_FACE_H
#define __BSP_FACE_H

#include "face_api.h"

typedef face_callback_t bsp_face_callback_t;

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct {
    void (*init)(bsp_face_callback_t);
    uint8_t (*loop)(void);
    uint8_t (*read_mode)(void);
    uint8_t (*set_mode)(uint8_t, bsp_face_callback_t, void*, uint8_t);
    uint8_t (*set_attr)(void*, uint8_t);
    uint8_t (*is_busy)(void);
} bsp_face_drive_io_t;

/***************Variable***************/


/***************Function***************/
const bsp_face_drive_io_t* bsp_face_get_driver(void);

/**************************************/

#endif /* __BSP_FACE_H */
