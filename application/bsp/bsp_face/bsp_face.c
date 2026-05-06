/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: bsp_face.c
 * Desc: 人脸识别模块板级抽象层
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 */
#include "bsp_face.h"

/***************Variable***************/
static const bsp_face_drive_io_t bsp_face = {
    .init = face_init_API,
    .loop = face_loop_API,
    .set_mode = face_set_mode_API,
    .read_mode = face_read_mode_API,
    .set_attr = face_set_attr_API,
    .is_busy = face_is_busy,
};

// ------------------------------------------
const bsp_face_drive_io_t* bsp_face_get_driver(void) {
    return &bsp_face;
}
