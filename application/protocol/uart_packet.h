/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: uart_packet.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#ifndef UART_PACKET_HH
#define UART_PACKET_HH

#include <stdint.h>
#include "hal_uart.h"
/***********Macro***********/

/***********Enum***********/

/***********Struct***********/
#pragma pack(1)
typedef struct
{
    uint16_t frame_start;        // 帧起始标志
    uint8_t control_version : 7; // 版本号
    uint8_t control_sm4 : 1;     // sm4加密控制位
    uint8_t tycmd;               // 命令字
    uint16_t tylength;           // 数据长度
    uint8_t TSN;                 // 时间序列号
    uint8_t cmd;                 // 命令字
    uint16_t length;             // 数据长度
    // union
    // {
    //     uint8_t data[12]; // 数据域
    // };
#if (BACK_UART_SEL == UART0)
    uint8_t payload[UART0_BUF_LEN]; // UART0对应长度
#elif (BACK_UART_SEL == UART1)
    uint8_t payload[UART1_BUF_LEN]; // UART1对应长度
#else
    #error "BACK_UART_SEL must be UART0 or UART1!"
#endif
} uart_packet_t;

#pragma pack()

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif // UART_PACKET_HH
