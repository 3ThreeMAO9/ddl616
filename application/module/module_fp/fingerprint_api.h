
/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: fingerprint_api.h
 * Desc: 指纹/ 指静脉模块对外API接口
 * Version: 1.0.0
 * Revision: XQ_R
 * Date: 2025-12-10
 * Note: 硬件实现在模块层实现
 */
#ifndef __FINGERPRINT_API_H
#define __FINGERPRINT_API_H

#include "fingerprint.h"

typedef fingerprint_callback_t fingerprint_api_callback_t;

/*****************Macro****************/


/*****************Enum*****************/


/****************Struct****************/
typedef struct {
    // 准备就绪
    uint8_t ready : 1;

    // 串口缓存
    uint8_t lenth;
    uint8_t buffer[FP_RX_BUFFER_SIZE];

    // 接收完成超时
    uint32_t time_out;

}fingerprint_uart_t;

typedef struct {
    fp_context_t ctx;               // 驱动层控制句柄
    fingerprint_uart_t uart;        // 串口接收

}fp_handle_t;

/***************Variable***************/


/***************Function***************/
uint8_t fp_loop_API(void);
void fp_init_API(fingerprint_api_callback_t callback);
uint8_t fp_set_mode_API(uint8_t mode, fingerprint_api_callback_t callback, void* params, uint8_t lenth);
uint8_t fp_set_attr_API(void* attr, uint8_t lenth);
uint8_t fp_control_led_API(uint8_t color);
uint8_t fp_is_wake_API(void);
uint8_t fp_is_busy_API(void);

/**************************************/

#endif /* __FINGERPRINT_API_H */
