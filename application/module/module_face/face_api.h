/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: face_api.h
 * Desc: 人脸识别模块对外API接口
 * Version: 1.0.0
 * Revision: James
 * Date: 2025-12-15
 * Note: 硬件实现在模块层实现
 */
#ifndef __FACE_API_H
#define __FACE_API_H

#include "face_st.h"

typedef face_callback_t face_api_callback_t;

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
// 人脸识别模块串口接收结构体
typedef struct {
    // 准备就绪标志位
    uint8_t ready : 1;

    // 串口缓存相关
    uint8_t lenth;
    uint8_t buffer[FACE_RX_BUFFER_SIZE];

    // 接收完成超时时间
    uint32_t time_out;

} face_uart_t;

// 人脸识别模块句柄结构体
typedef struct {
    face_context_t ctx;               // 驱动层控制句柄
    face_uart_t uart;                 // 串口接收

} face_handle_t;

/***************Variable***************/


/***************Function***************/
uint8_t face_loop_API(void);
void face_init_API(face_api_callback_t callback);
uint8_t face_read_mode_API(void);
uint8_t face_set_mode_API(uint8_t mode, face_api_callback_t callback, void* params, uint8_t lenth);
uint8_t face_set_attr_API(void* attr, uint8_t lenth);

/**************************************/

#endif /* __FACE_API_H */
