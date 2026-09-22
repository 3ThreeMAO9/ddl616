/**
 * Copyright (c) 2025 GZ-OB, All rights reserved.
 * File name: msg_protocol.h
 * Desc:
 * Version: 1.0.0
 * Revision: James_Zhang
 * Date: 2025-12-02
 */

#ifndef MSG_PROTOCOL_HH
#define MSG_PROTOCOL_HH

#include <stdint.h>
#include "uart_protocol.h"
#include "uart_packet.h"
#include "encrypt.h"
#include "config.h"
/***********Macro***********/

/***********Enum***********/
typedef enum{
    EVENT_KEY_ID_NONE    = 0x00,
    EVENT_KEY_RESET_ID   = 0x01,
    EVENT_KEY_SETTING_ID = 0x02,
}event_key_id_t;
typedef enum{
    EVENT_KEY_REPORT_NONE   = 0x00,   /* 无上报 */
    EVENT_KEY_REPORT_SINGLE = 0x01,   /* 单击 */
    EVENT_KEY_REPORT_DOUBLE = 0x02,   /* 双击 */
    EVENT_KEY_REPORT_LONG   = 0x03,   /* 长按 */
}event_key_report_t;


/***********Struct***********/
#pragma pack(1)


/*********************接收处理的结构体********************/
typedef struct
{
    uint8_t event_key_id;
    uint8_t event_key_report;
} frame_key_t;


/*********************发送处理的结构体********************/
typedef struct {
    uint8_t status;
} frame_ack_def_t;

typedef struct {
    uint8_t cnt;            // 响几次
    uint32_t timeout;       // 间隔多久  单位：1ms
} frame_bell_def_t;



/**************************************************/

#pragma pack()

/***********Variable***********/

/***********Function***********/
void uart_msg_bell(uint8_t cnt, uint32_t timeout);


void uart_msg_ack(uint8_t cmd, uint8_t status);

/*****************************/

#endif // MSG_PROTOCOL_HH
