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

/***********Struct***********/
#pragma pack(1)
typedef struct
{
    uint8_t  mark;          // 0xA5
    uint8_t  attr;          // bit0-3=地址, bit4-5=加密类型
    uint16_t checksum1;     // 头部校验 = SUM(序号~数据内容)
    uint16_t sn;            // 序列号
    uint8_t  cmd;           // 命令字
    uint8_t  random;        // 随机数
    uint16_t checksum2;     // 数据校验 = SUM(加密前数据内容)
    uint16_t length;        // 数据长度（payload 字节数）
    uint8_t  payload[];     // 数据内容（加密后）
} uart_packet_t;

#pragma pack()

/***********Enum***********/
#define LOCK_PACKET_MARK                (0xA5)
#define LOCK_PACKET_HEAD_SIZE           (sizeof(uart_packet_t))   // 12 字节

// attr 位定义
#define LOCK_PACKET_ATTR_ADDR_MASK      (0x0F)
#define LOCK_PACKET_ATTR_ENC_MASK       (0x30)
#define LOCK_PACKET_ATTR_ENC_SHIFT      (4)

// 地址定义
#define LOCK_PACKET_ADDR_TEST           (0)  // 测试架
#define LOCK_PACKET_ADDR_FRONT          (1)  // 前板
#define LOCK_PACKET_ADDR_BACK           (2)  // 背板
#define LOCK_PACKET_ADDR_WIFI           (3)  // 猫眼

// 加密类型
#define LOCK_PACKET_ENCRYPT_TYPE_NONE   (0)
#define LOCK_PACKET_ENCRYPT_TYPE_XOR    (1)

/***********Variable***********/

/***********Function***********/

/*****************************/

#endif // UART_PACKET_HH
